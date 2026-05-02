/* codegen.c
 *
 * Simple 3-address code generator utilities:
 *  - temp & label generation
 *  - emit instructions
 *  - backpatching support
 *  - print_code for debugging/output
 *
 * Keep this as the single place that defines new_temp/new_label.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- safe strdup replacement (avoids implicit-declaration issues) --- */
static char *my_strdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (!p) return NULL;
    memcpy(p, s, n);
    return p;
}

/* --- Instruction representation --- */
typedef struct {
    char *op;     /* operation, e.g., =, +, -, * , /, ifgoto, goto, label, print */
    char *arg1;   /* first operand (or condition for ifgoto) */
    char *arg2;   /* second operand (optional) */
    char *res;    /* result or destination (temp, label) */
} Instr;

/* Dynamic array of instructions */
static Instr *instrs = NULL;
static int instr_capacity = 0;
static int instr_count = 0;

/* Ensure capacity for new instructions */
static void ensure_instr_capacity(void) {
    if (instr_capacity == 0) {
        instr_capacity = 64;
        instrs = (Instr *)calloc(instr_capacity, sizeof(Instr));
    } else if (instr_count >= instr_capacity) {
        instr_capacity *= 2;
        instrs = (Instr *)realloc(instrs, instr_capacity * sizeof(Instr));
    }
}

/* Allocate and append an instruction, returns its index */
int next_instr(void) {
    ensure_instr_capacity();
    return instr_count;
}

/* Emit an instruction (copies strings) */
void emit(const char *op, const char *a, const char *b, const char *res) {
    ensure_instr_capacity();
    Instr *I = &instrs[instr_count++];
    I->op  = op  ? my_strdup(op)  : NULL;
    I->arg1= a   ? my_strdup(a)   : NULL;
    I->arg2= b   ? my_strdup(b)   : NULL;
    I->res = res ? my_strdup(res) : NULL;
}

/* Backpatch: replace the res (or arg1/arg2 depending on design) of an instruction
   with a label string. For simplicity we patch the 'res' field at instr_index. */
void backpatch(int instr_index, const char *label) {
    if (instr_index < 0 || instr_index >= instr_count) return;
    Instr *I = &instrs[instr_index];
    /* free old res if present */
    if (I->res) {
        free(I->res);
        I->res = NULL;
    }
    I->res = my_strdup(label);
}

/* Free all allocated instruction strings and array */
void free_code(void) {
    if (!instrs) return;
    for (int i = 0; i < instr_count; ++i) {
        free(instrs[i].op);
        free(instrs[i].arg1);
        free(instrs[i].arg2);
        free(instrs[i].res);
    }
    free(instrs);
    instrs = NULL;
    instr_capacity = instr_count = 0;
}

/* Print the generated 3-address code in human-readable form */
void print_code(void) {
    for (int i = 0; i < instr_count; ++i) {
        Instr *I = &instrs[i];
        /* print index for reference (useful for backpatching debugging) */
        printf("%3d: ", i);
        if (!I->op) {
            printf("<empty>\n");
            continue;
        }

        /* handle common ops */
        if (strcmp(I->op, "label") == 0) {
            printf("%s:\n", I->res ? I->res : "(nil)");
        } else if (strcmp(I->op, "goto") == 0) {
            printf("goto %s\n", I->res ? I->res : "(nil)");
        } else if (strcmp(I->op, "ifgoto") == 0) {
            /* arg1 = condition (e.g. t1 < t2), res = label */
            if (I->arg1 && I->res)
                printf("if %s goto %s\n", I->arg1, I->res);
            else
                printf("if %s goto %s\n", I->arg1 ? I->arg1 : "(nil)", I->res ? I->res : "(nil)");
        } else if (strcmp(I->op, "print") == 0) {
            printf("print %s\n", I->arg1 ? I->arg1 : "(nil)");
        } else if (strcmp(I->op, "=") == 0) {
            printf("%s = %s\n", I->res ? I->res : "(nil)", I->arg1 ? I->arg1 : "(nil)");
        } else if (strcmp(I->op, "+") == 0 || strcmp(I->op, "-") == 0
                || strcmp(I->op, "*") == 0 || strcmp(I->op, "/") == 0
                || strcmp(I->op, "<") == 0 || strcmp(I->op, ">") == 0
                || strcmp(I->op, "==") == 0 || strcmp(I->op, "!=") == 0
                || strcmp(I->op, "<=") == 0 || strcmp(I->op, ">=") == 0) {
            printf("%s = %s %s %s\n",
                   I->res ? I->res : "(nil)",
                   I->arg1 ? I->arg1 : "(nil)",
                   I->op,
                   I->arg2 ? I->arg2 : "(nil)");
        } else {
            /* generic */
            printf("%s %s %s %s\n",
                   I->op,
                   I->arg1 ? I->arg1 : "",
                   I->arg2 ? I->arg2 : "",
                   I->res  ? I->res  : "");
        }
    }
}

/* --- temp and label generation --- */
char *new_temp(void) {
    static int cnt = 0;
    char buf[32];
    sprintf(buf, "t%d", ++cnt);
    return my_strdup(buf);
}

char *new_label(void) {
    static int lbl = 0;
    char buf[32];
    sprintf(buf, "L%d", ++lbl);
    return my_strdup(buf);
}

/* --- convenience helper to emit arithmetic binary op:
   Example usage from parser actions:
       char *t1 = new_temp();
       emit("+", "a", "b", t1);
       $$ = t1;
*/
char *emit_binary_op(const char *op, const char *left, const char *right) {
    char *res = new_temp();
    emit(op, left, right, res);
    return res;
}

/* convenience: emit assignment:  x = y */
void emit_assign(const char *dest, const char *src) {
    emit("=", src, NULL, dest);
}

/* convenience: emit a label */
void emit_label(const char *label) {
    emit("label", NULL, NULL, label);
}

/* convenience: emit unconditional goto (res = label) */
void emit_goto(const char *label) {
    emit("goto", NULL, NULL, label);
}

/* convenience: emit conditional goto where condition is provided as a string
   e.g., condition = "t1 < t2" or "x == 0", res = label to jump to */
int emit_ifgoto(const char *condition, const char *label) {
    int idx = next_instr();
    emit("ifgoto", condition, NULL, label);
    return idx;
}

/* convenience: emit print */
void emit_print(const char *what) {
    emit("print", what, NULL, NULL);
}

/* Helper to get current instruction count (useful for patch points) */
int code_size(void) {
    return instr_count;
}

/* Example: backpatch a list of indices (simple API) */
void backpatch_list(int *list, int list_len, const char *label) {
    for (int i = 0; i < list_len; ++i) {
        backpatch(list[i], label);
    }
}

/* Exported for other modules that may want to free generated code before exit */
void cleanup_code_generator(void) {
    free_code();
}

/* Optionally, you might want to write the code to a file */
int write_code_to_file(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return -1;
    for (int i = 0; i < instr_count; ++i) {
        Instr *I = &instrs[i];
        /* print same textual representation as print_code but to file */
        if (!I->op) {
            fprintf(f, "%3d: <empty>\n", i);
            continue;
        }
        if (strcmp(I->op, "label") == 0) {
            fprintf(f, "%s:\n", I->res ? I->res : "(nil)");
        } else if (strcmp(I->op, "goto") == 0) {
            fprintf(f, "goto %s\n", I->res ? I->res : "(nil)");
        } else if (strcmp(I->op, "ifgoto") == 0) {
            fprintf(f, "if %s goto %s\n", I->arg1 ? I->arg1 : "(nil)", I->res ? I->res : "(nil)");
        } else if (strcmp(I->op, "print") == 0) {
            fprintf(f, "print %s\n", I->arg1 ? I->arg1 : "(nil)");
        } else if (strcmp(I->op, "=") == 0) {
            fprintf(f, "%s = %s\n", I->res ? I->res : "(nil)", I->arg1 ? I->arg1 : "(nil)");
        } else {
            fprintf(f, "%s %s %s %s\n",
                    I->op,
                    I->arg1 ? I->arg1 : "",
                    I->arg2 ? I->arg2 : "",
                    I->res  ? I->res  : "");
        }
    }
    fclose(f);
    return 0;
}

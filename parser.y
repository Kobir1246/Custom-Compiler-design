%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Lexer */
int yylex(void);
int yyparse(void);
void yyerror(const char *s);

/* ================= SYMBOL TABLE (external) ================= */
typedef enum { TY_INT, TY_FLOAT, TY_UNKNOWN } VarType;
extern void sym_add(const char *name, VarType type);
extern VarType sym_get_type(const char *name);


extern char *new_temp(void);


static int label_count = 0;
static char *new_label(void) {
    char buf[32];
    sprintf(buf, "L%d", label_count++);
    return strdup(buf);
}


int parse_program(void) {
    return yyparse();
}

/* semantic error helper */
static void semantic_error(const char *msg, const char *id) {
    fprintf(stderr, "Semantic Error: %s '%s'\n", msg, id);
}
%}

/* ================= SEMANTIC VALUES ================= */
%union {
    int ival;
    double fval;
    char *id;
}

/* ================= TOKENS ================= */
%token <id> ID
%token <ival> INT_LITERAL
%token <fval> FLOAT_LITERAL

%token KW_INT KW_FLOAT KW_PRINT KW_IF KW_ELSE
%token SEMI COMMA LP RP LB RB ASSIGN
%token PLUS MINUS MUL DIV
%token EQ NEQ LT GT LE GE

/* ================= PRECEDENCE ================= */
%nonassoc LOWER_THAN_ELSE
%nonassoc KW_ELSE


%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left MUL DIV

%type <id> expr term factor cond

%%

program:
    stmt_list
    ;

stmt_list:
      /* empty */
    | stmt_list stmt
    ;

stmt:
      KW_INT ID SEMI {
            if (sym_get_type($2) != TY_UNKNOWN)
                semantic_error("Redeclaration of variable", $2);
            else
                sym_add($2, TY_INT);
            free($2);
        }
    | KW_FLOAT ID SEMI {
            if (sym_get_type($2) != TY_UNKNOWN)
                semantic_error("Redeclaration of variable", $2);
            else
                sym_add($2, TY_FLOAT);
            free($2);
        }
    | KW_INT ID ASSIGN expr SEMI {
            if (sym_get_type($2) != TY_UNKNOWN)
                semantic_error("Redeclaration of variable", $2);
            else
                sym_add($2, TY_INT);
            printf("%s = %s\n", $2, $4);
            free($2); free($4);
        }
    | KW_FLOAT ID ASSIGN expr SEMI {
            if (sym_get_type($2) != TY_UNKNOWN)
                semantic_error("Redeclaration of variable", $2);
            else
                sym_add($2, TY_FLOAT);
            printf("%s = %s\n", $2, $4);
            free($2); free($4);
        }
    | ID ASSIGN expr SEMI {
            if (sym_get_type($1) == TY_UNKNOWN)
                semantic_error("Undeclared variable", $1);
            else
                printf("%s = %s\n", $1, $3);
            free($1); free($3);
        }
    | KW_PRINT LP expr RP SEMI {
            printf("print %s\n", $3);
            free($3);
        }

    /* -------- IF -------- */
| KW_IF LP cond RP LB stmt_list RB %prec LOWER_THAN_ELSE {
        char *L_true = new_label();
        char *L_end  = new_label();

        printf("if %s goto %s\n", $3, L_true);
        printf("goto %s\n", L_end);
        printf("%s:\n", L_true);
        printf("%s:\n", L_end);

        free($3); free(L_true); free(L_end);
    }

/* -------- IF ELSE -------- */
| KW_IF LP cond RP LB stmt_list RB
  KW_ELSE LB stmt_list RB {
        char *L_true  = new_label();
        char *L_false = new_label();
        char *L_end   = new_label();

        printf("if %s goto %s\n", $3, L_true);
        printf("goto %s\n", L_false);

        printf("%s:\n", L_true);
        printf("goto %s\n", L_end);

        printf("%s:\n", L_false);
        printf("%s:\n", L_end);

        free($3); free(L_true); free(L_false); free(L_end);
    }

    ;

expr:
      expr PLUS term {
            char *t = new_temp();
            printf("%s = %s + %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | expr MINUS term {
            char *t = new_temp();
            printf("%s = %s - %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | term { $$ = $1; }
    ;

term:
      term MUL factor {
            char *t = new_temp();
            printf("%s = %s * %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | term DIV factor {
            char *t = new_temp();
            printf("%s = %s / %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | factor { $$ = $1; }
    ;

factor:
      ID {
            if (sym_get_type($1) == TY_UNKNOWN)
                semantic_error("Use of undeclared variable", $1);
            $$ = $1;
        }
    | INT_LITERAL {
            char buf[32];
            sprintf(buf, "%d", $1);
            $$ = strdup(buf);
        }
    | FLOAT_LITERAL {
            char buf[32];
            sprintf(buf, "%g", $1);
            $$ = strdup(buf);
        }
    | LP expr RP { $$ = $2; }
    ;

cond:
      expr LT expr {
            char *t = new_temp();
            printf("%s = %s < %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | expr GT expr {
            char *t = new_temp();
            printf("%s = %s > %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | expr LE expr {
            char *t = new_temp();
            printf("%s = %s <= %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | expr GE expr {
            char *t = new_temp();
            printf("%s = %s >= %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | expr EQ expr {
            char *t = new_temp();
            printf("%s = %s == %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    | expr NEQ expr {
            char *t = new_temp();
            printf("%s = %s != %s\n", t, $1, $3);
            free($1); free($3);
            $$ = t;
        }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}

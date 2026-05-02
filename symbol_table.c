// symbol_table.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TY_INT, TY_FLOAT, TY_UNKNOWN } VarType;

typedef struct Sym {
    char *name;
    VarType type;
    struct Sym *next;
} Sym;

static Sym *symtab = NULL;

void sym_add(const char *name, VarType type) {
    // check existing
    Sym *p = symtab;
    while(p) { if(strcmp(p->name, name)==0) { p->type = type; return; } p = p->next; }
    Sym *n = malloc(sizeof(Sym));
    n->name = strdup(name);
    n->type = type;
    n->next = symtab;
    symtab = n;
}

VarType sym_get_type(const char *name) {
    Sym *p = symtab;
    while(p) { if(strcmp(p->name, name)==0) return p->type; p = p->next; }
    return TY_UNKNOWN;
}

const char *type_name(VarType t) {
    if(t==TY_INT) return "int";
    if(t==TY_FLOAT) return "float";
    return "unknown";
}

void sym_free_all() {
    Sym *p = symtab;
    while(p) {
        Sym *nx = p->next;
        free(p->name);
        free(p);
        p = nx;
    }
    symtab = NULL;
}

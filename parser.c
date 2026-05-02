// parser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

// forward lexer functions
void lexer_init(const char *fname);
Token next_token();
void lexer_close();
void free_token(Token *t);

// symbol table
// declare functions from symbol_table.c
typedef enum { TY_INT, TY_FLOAT, TY_UNKNOWN } VarType;
void sym_add(const char *name, VarType type);
VarType sym_get_type(const char *name);
const char *type_name(VarType t);

// codegen
char *new_temp();
void emit(const char *, const char *, const char *);

// parser state
static Token cur;
static Token lookahead;

static void advance() {
    if(cur.lexeme) free_token(&cur);
    cur = lookahead;
    lookahead = next_token();
}

static int accept_type(TokenType tt, const char *lex) {
    if(lookahead.type == tt && (lex == NULL || (lookahead.lexeme && strcmp(lookahead.lexeme, lex)==0)) ) {
        advance(); return 1;
    }
    return 0;
}

static void expect(TokenType tt, const char *lex) {
    if(!accept_type(tt, lex)) {
        fprintf(stderr, "Syntax error (line %d): expected %s %s but got %s\n",
                lookahead.line, lex?lex:"", (tt==TOK_PUNC?"PUNC":"",""), lookahead.lexeme?lookahead.lexeme:"(EOF)");
        exit(1);
    }
}

// forward decls
char *parse_expr(); // returns name of temp or literal as string
char *parse_term();
char *parse_factor();

void parse_declaration() {
    // current lookahead at 'int' or 'float'
    char *kw = strdup(lookahead.lexeme);
    advance(); // consume keyword
    if(lookahead.type != TOK_ID) { fprintf(stderr, "Expected identifier after type\n"); exit(1); }
    char *name = strdup(lookahead.lexeme);
    advance(); // id
    // optional initialization
    if(accept_type(TOK_OP, "=")) {
        char *r = parse_expr();
        // emit assignment: name = r
        printf("%s = %s\n", name, r);
        free(r);
    }
    expect(TOK_PUNC, ";");
    // add symbol
    if(strcmp(kw,"int")==0) sym_add(name, TY_INT);
    else if(strcmp(kw,"float")==0) sym_add(name, TY_FLOAT);
    free(kw); free(name);
}

void parse_statement();

void parse_block() {
    expect(TOK_PUNC, "{");
    while(!(lookahead.type==TOK_PUNC && lookahead.lexeme && strcmp(lookahead.lexeme,"}")==0)) {
        parse_statement();
    }
    expect(TOK_PUNC, "}");
}

void parse_if() {
    // lookahead is 'if'
    advance();
    expect(TOK_PUNC, "(");
    char *lhs = parse_expr();
    // We will just assume comparison operators are part of expression; for simplicity,
    expect(TOK_PUNC, ")");
    // parse block
    if(lookahead.type==TOK_PUNC && lookahead.lexeme && strcmp(lookahead.lexeme,"{")==0) {
        parse_block();
    } else {
        parse_statement();
    }
    // (no else handling for brevity)
    free(lhs);
}

void parse_statement() {
    if(lookahead.type==TOK_KEYWORD) {
        if(strcmp(lookahead.lexeme,"int")==0 || strcmp(lookahead.lexeme,"float")==0) {
            parse_declaration();
            return;
        } else if(strcmp(lookahead.lexeme,"print")==0) {
            advance(); // consume print
            expect(TOK_PUNC, "(");
            char *r = parse_expr();
            expect(TOK_PUNC, ")");
            expect(TOK_PUNC, ";");
            printf("print %s\n", r);
            free(r);
            return;
        } else if(strcmp(lookahead.lexeme,"if")==0) {
            parse_if();
            return;
        }
    }

    // assignment or expression statement
    if(lookahead.type==TOK_ID) {
        char *id = strdup(lookahead.lexeme);
        advance();
        if(accept_type(TOK_OP, "=")) {
            char *r = parse_expr();
            expect(TOK_PUNC, ";");
            printf("%s = %s\n", id, r);
            free(r);
            free(id);
            return;
        } else {
            fprintf(stderr, "Unexpected token after identifier\n");
            exit(1);
        }
    }

    // empty or unknown
    if(lookahead.type==TOK_PUNC && lookahead.lexeme && strcmp(lookahead.lexeme,";")==0) {
        advance();
        return;
    }

    fprintf(stderr, "Unknown statement start: %s\n", lookahead.lexeme?lookahead.lexeme:"(EOF)");
    exit(1);
}

// precedence: expr (+ -) term (* /) factor
char *parse_expr() {
    char *left = parse_term();
    while(lookahead.type==TOK_OP && (strcmp(lookahead.lexeme,"+")==0 || strcmp(lookahead.lexeme,"-")==0
          || strcmp(lookahead.lexeme,"==")==0 || strcmp(lookahead.lexeme,"!=")==0
          || strcmp(lookahead.lexeme,">")==0 || strcmp(lookahead.lexeme,"<")==0
          || strcmp(lookahead.lexeme,">=")==0 || strcmp(lookahead.lexeme,"<=")==0)) {
        char op[4]; strcpy(op, lookahead.lexeme);
        advance();
        char *right = parse_term();
        char *tmp = new_temp();
        printf("%s = %s %s %s\n", tmp, left, op, right);
        free(left); free(right);
        left = tmp;
    }
    return left;
}

char *parse_term() {
    char *left = parse_factor();
    while(lookahead.type==TOK_OP && (strcmp(lookahead.lexeme,"*")==0 || strcmp(lookahead.lexeme,"/")==0)) {
        char op[3]; strcpy(op, lookahead.lexeme);
        advance();
        char *right = parse_factor();
        char *tmp = new_temp();
        printf("%s = %s %s %s\n", tmp, left, op, right);
        free(left); free(right);
        left = tmp;
    }
    return left;
}

char *parse_factor() {
    if(lookahead.type == TOK_INT || lookahead.type == TOK_FLOAT) {
        char *lit = strdup(lookahead.lexeme);
        advance();
        return lit;
    } else if(lookahead.type == TOK_ID) {
        char *id = strdup(lookahead.lexeme);
        advance();
        return id;
    } else if(lookahead.type == TOK_PUNC && strcmp(lookahead.lexeme,"(")==0) {
        advance();
        char *r = parse_expr();
        expect(TOK_PUNC, ")");
        return r;
    } else {
        fprintf(stderr, "Unexpected factor: %s (line %d)\n", lookahead.lexeme?lookahead.lexeme:"(EOF)", lookahead.line);
        exit(1);
    }
}

int parse_program() {
    // prime tokens
    cur.lexeme = NULL;
    lookahead = next_token();
    while(lookahead.type != TOK_EOF) {
        parse_statement();
    }
    return 0;
}

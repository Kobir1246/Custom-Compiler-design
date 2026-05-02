// token.h
#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>

typedef enum {
    TOK_EOF,
    TOK_KEYWORD,
    TOK_ID,
    TOK_INT,
    TOK_FLOAT,
    TOK_OP,
    TOK_PUNC,
    TOK_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
    int line;
} Token;

void free_token(Token *t);

#endif

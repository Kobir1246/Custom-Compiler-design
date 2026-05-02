// main.c
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

// declarations from lexer and other modules
void lexer_init(const char *fname);
Token next_token();
void lexer_close();
void sym_free_all();

int parse_program();

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s input.mc\n", argv[0]);
        return 1;
    }
    lexer_init(argv[1]);
    parse_program();
    lexer_close();
    sym_free_all();
    return 0;
}

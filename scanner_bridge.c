/* scanner_bridge.c
 * Small bridge between your main.c and the Flex scanner (lex.yy.c).
 * - implements lexer_init(const char*) and lexer_close()
 * - declares the Flex yylex() so the parser finds it at link time
 */

#include <stdio.h>
#include <stdlib.h>

/* yyin is the FILE* used by Flex to read input */
extern FILE *yyin;

/* yylex is provided by lex.yy.c (Flex). Declare it so the linker knows it. */
int yylex(void);

/* Initialize scanner to read from filename. Returns 0 on success, -1 on failure. */
int lexer_init(const char *filename) {
    if (!filename) return -1;
    yyin = fopen(filename, "r");
    if (!yyin) {
        perror("fopen");
        return -1;
    }
    return 0;
}

/* Close the scanner input */
void lexer_close(void) {
    if (yyin) {
        fclose(yyin);
        yyin = NULL;
    }
}


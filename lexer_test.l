%option noyywrap
%{
#include "parser.tab.h"   /* produced by bison -d parser.y */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern YYSTYPE yylval;
%}

/* direct patterns instead of macro+quantifier shorthands for portability */
DIGIT   [0-9]
LETTER  [a-zA-Z_]
ALNUM   [a-zA-Z0-9_]

%%

[ \t\r\n]+                 { /* skip whitespace */ }

/* single-line C++ style comment */
\/\/[^\n]*                 { /* skip comment */ }

/* keywords (use word boundaries by matching identifier pattern) */
"int"                      { return KW_INT; }
"float"                    { return KW_FLOAT; }
"print"                    { return KW_PRINT; }
"if"                       { return KW_IF; }
"else"                     { return KW_ELSE; }

/* multi-char operators */
"=="                       { return EQ; }
"!="                       { return NEQ; }
"<="                       { return LE; }
">="                       { return GE; }

/* single char punctuation/operators */
";"                        { return SEMI; }
","                        { return COMMA; }
"("                        { return LP; }
")"                        { return RP; }
"{"                        { return LB; }
"}"                        { return RB; }
"+"                        { return PLUS; }
"-"                        { return MINUS; }
"*"                        { return MUL; }
"/"                        { return DIV; }

"<"                        { return LT; }
">"                        { return GT; }

"="                        { return ASSIGN; }

/* float literal: one or more digits, a dot, one or more digits */
{DIGIT}+\.{DIGIT}+         {
                             yylval.fval = atof(yytext);
                             return FLOAT_LITERAL;
                           }

/* integer literal */
{DIGIT}+                   {
                             yylval.ival = atoi(yytext);
                             return INT_LITERAL;
                           }

/* identifier */
{LETTER}{ALNUM}*           {
                             yylval.id = strdup(yytext);
                             return ID;
                           }

/* any other single character — ignore or return it (here we ignore) */
.                          { /* skip unknown character */ }

%%

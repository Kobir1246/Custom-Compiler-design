%option noyywrap
%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern YYSTYPE yylval;
%}

%%

[ \t\r\n]+                 { /* skip whitespace */ }

"//"[^\n]*                 { /* skip single-line comments */ }

"int"                      { return KW_INT; }
"float"                    { return KW_FLOAT; }
"print"                    { return KW_PRINT; }
"if"                       { return KW_IF; }
"else"                     { return KW_ELSE; }

"=="                       { return EQ; }
"!="                       { return NEQ; }
"<="                       { return LE; }
">="                       { return GE; }

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

[0-9]+\.[0-9]+             { yylval.fval = atof(yytext); return FLOAT_LITERAL; }
[0-9]+                     { yylval.ival = atoi(yytext); return INT_LITERAL; }

[a-zA-Z_][a-zA-Z0-9_]*     { yylval.id = strdup(yytext); return ID; }

.                          { /* ignore any other character */ }

%%

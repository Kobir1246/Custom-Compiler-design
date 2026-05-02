%option noyywrap
%{
#include <stdio.h>
%}
%%
"abc" { return 1; }
.     { /* skip */ }
%%

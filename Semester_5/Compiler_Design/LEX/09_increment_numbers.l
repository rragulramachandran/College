%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%

[+-]?(([0-9]+\.[0-9]*|\.[0-9]+)([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+)    {
    printf("%g", atof(yytext) + 0.5);
}

[+-]?[0-9]+    {
    printf("%ld", strtol(yytext, NULL, 10) + 1);
}

.|\n    {
    ECHO;
}

%%

int main()
{
    yylex();
    return 0;
}

int yywrap()
{

    return 1;
}

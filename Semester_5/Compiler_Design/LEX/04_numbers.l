%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%

[+-]?([0-9]+\.[0-9]*|\.[0-9]+|[0-9]+\/[0-9]+)    {
    printf("Fraction: %s\n", yytext);
}

-[0-9]+    {
    printf("Negative number: %s\n", yytext);
}

\+?[0-9]+    {
    if (strtol(yytext, NULL, 10) == 0)
    {
        printf("Zero: %s\n", yytext);
    }
    else
    {
        printf("Positive number: %s\n", yytext);
    }
}

[ \t\r\n,]+    {
    /* Ignore this match. */
}

.    {
    printf("Invalid input: %s\n", yytext);
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

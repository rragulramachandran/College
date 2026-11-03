%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%

[sS][eE][lL][eE][cC][tT]|[fF][rR][oO][mM]|[wW][hH][eE][rR][eE]|[gG][rR][oO][uU][pP]|[bB][yY]|[aA][nN][dD]|[oO][rR]    {
    printf("Keyword: %s\n", yytext);
}

[lL][iI][kK][eE]|"<="|">="|"<>"|"!="|[=><]    {
    printf("Operator: %s\n", yytext);
}

\'([^']|\'\')*\'    {
    printf("String: %s\n", yytext);
}

[0-9]+(\.[0-9]+)?    {
    printf("Number: %s\n", yytext);
}

[a-zA-Z_][a-zA-Z0-9_]*    {
    printf("Identifier: %s\n", yytext);
}

[*,;().]    {
    printf("Punctuation: %s\n", yytext);
}

[ \t\r\n]+    {
    /* Ignore this match. */
}

.    {
    printf("Unknown: %s\n", yytext);
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

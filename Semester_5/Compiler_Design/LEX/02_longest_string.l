%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char longest[100] = "";
int longest_length = 0;
%}

%%

[^ \t\r\n\f\v]+    {
    if (yyleng > longest_length)
    {
        strcpy(longest, yytext);
        longest_length = yyleng;
    }
}

[ \t\r\n\f\v]+    {
    /* Ignore this match. */
}

%%

int main()
{
    yylex();
    return 0;
}

int yywrap()
{
    printf("Longest string: %s\nLength: %d\n", longest ? longest : "", longest_length);
    free(longest);
    return 1;
}

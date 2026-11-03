%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%

[ \t]{2,}    {
    putchar(' ');
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

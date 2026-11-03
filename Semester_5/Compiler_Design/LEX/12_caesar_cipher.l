%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%

[A-Z]    {
    putchar('A' + (yytext[0] - 'A' + 3) % 26);
}

[a-z]    {
    putchar('a' + (yytext[0] - 'a' + 3) % 26);
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

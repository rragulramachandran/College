%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
%}

%%

[a-zA-Z]+    {
    if (yyleng == 5)
    {
        yytext[0] = toupper((unsigned char)yytext[0]);
    }
    ECHO;
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

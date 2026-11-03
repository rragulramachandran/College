%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%

(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)    {
    printf("<kw,%s>", yytext);
}

[a-zA-Z_][a-zA-Z0-9_]*    {
    printf("<id,%s>", yytext);
}

[0-9]+\.[0-9]+    {
    printf("%sf", yytext);
}

[0-9]+    {
    printf("%si", yytext);
}

"=="|"!="|"<="|">="|"++"|"--"|[+*/%=<>!-]    {
    printf("<op,%s>", yytext);
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

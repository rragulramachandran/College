%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
long words = 0, characters = 0, lines = 0;
%}

%%

\n    {
    lines++;
    characters++;
}

[ \t\r\f\v]+    {
    characters += yyleng;
}

[^ \t\r\n\f\v]+    {
    words++;
    characters += yyleng;
}

%%

int main()
{
    yylex();
    return 0;
}

int yywrap()
{
    printf("Words: %ld\nCharacters: %ld\nLines: %ld\n", words, characters, lines);
    return 1;
}

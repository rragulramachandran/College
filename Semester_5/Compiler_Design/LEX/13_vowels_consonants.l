%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int vowels = 0, consonants = 0;
%}

%%

[aeiouAEIOU]    {
    vowels++;
}

[a-zA-Z]    {
    consonants++;
}

.|\n    {
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
    printf("Vowels: %d\nConsonants: %d\n", vowels, consonants);
    return 1;
}

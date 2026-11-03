%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%x LINK

%%

(https?:\/\/|ftp:\/\/|www\.)[^ \t\r\n<>\"\']+    {
    printf("%s\n", yytext);
}

[hH][rR][eE][fF][ \t\r\n]*=[ \t\r\n]*|[sS][rR][cC][ \t\r\n]*=[ \t\r\n]*    {
    BEGIN(LINK);
}

<LINK>\"[^\"]*\"|\'[^']*\'    {
    printf("%.*s\n", (int)yyleng - 2, yytext + 1);
    BEGIN(INITIAL);
}

<LINK>[^ \t\r\n>]+    {
    printf("%s\n", yytext);
    BEGIN(INITIAL);
}

<LINK>[ \t\r\n]+    {
    /* Ignore this match. */
}

<LINK>">"    {
    BEGIN(INITIAL);
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

    return 1;
}

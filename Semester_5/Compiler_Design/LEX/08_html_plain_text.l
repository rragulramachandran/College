%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%x TAG COMMENT

%%

"<!--"    {
    BEGIN(COMMENT);
}

<COMMENT>"-->"    {
    BEGIN(INITIAL);
}

<COMMENT>(.|\n)    {
    /* Ignore this match. */
}

"<"    {
    BEGIN(TAG);
}

<TAG>\"[^\"]*\"|\'[^']*\'    {
    /* Ignore this match. */
}

<TAG>">"    {
    BEGIN(INITIAL);
}

<TAG>(.|\n)    {
    /* Ignore this match. */
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

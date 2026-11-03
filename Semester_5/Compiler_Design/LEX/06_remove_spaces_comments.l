%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%x COMMENT

%%

"/*"    {
    BEGIN(COMMENT);
}

<COMMENT>"*/"    {
    BEGIN(INITIAL);
}

<COMMENT>(.|\n)    {
    /* Ignore this match. */
}

[ \t\r\n\f\v]+    {
    /* Ignore this match. */
}

.    {
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

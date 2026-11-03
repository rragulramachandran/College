%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%%

"<="    {
    puts("Less than or equal to");
}

">="    {
    puts("Greater than or equal to");
}

"=="    {
    puts("Equal to");
}

"!="    {
    puts("Not equal to");
}

"<"    {
    puts("Less than");
}

">"    {
    puts("Greater than");
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

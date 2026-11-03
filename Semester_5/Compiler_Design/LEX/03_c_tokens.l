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

"//"[^\n]*    {
    /* Ignore this match. */
}

(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)    {
    printf("Keyword: %s\n", yytext);
}

[a-zA-Z_][a-zA-Z0-9_]*    {
    printf("Identifier: %s\n", yytext);
}

([0-9]+\.[0-9]*|\.[0-9]+)([eE][+-]?[0-9]+)?|[0-9]+([eE][+-]?[0-9]+)?    {
    printf("Constant: %s\n", yytext);
}

\"([^\\\"\n]|\\.)*\"|\'([^\\\'\n]|\\.)\'    {
    printf("Constant: %s\n", yytext);
}

"++"|"--"|"=="|"!="|"<="|">="|"&&"|"||"|"+="|"-="|"*="|"/="|"%="|"<<"|">>"|"->"|[+*/%=!<>&|^~?:-]    {
    printf("Operator: %s\n", yytext);
}

[(){}\[\],;.]    {
    printf("Punctuation: %s\n", yytext);
}

[ \t\r\n]+    {
    /* Ignore this match. */
}

.    {
    printf("Unknown: %s\n", yytext);
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

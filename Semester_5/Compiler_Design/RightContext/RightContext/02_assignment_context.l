%{
#include <stdio.h>
%}


id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

{id}/{ws}"=" { printf("ASSIGNMENT VARIABLE : %s\n", yytext); }
{id}/{ws}("=="|"!="|"<="|">="|"++"|"--"|"+="|"-="|"*="|"/="|[+*/%<>!&|^-]) { printf("OPERATOR CONTEXT VARIABLE : %s\n", yytext); }
{id}        { printf("IDENTIFIER : %s\n", yytext); BEGIN(INITIAL); }
[0-9]+      { printf("NUMBER : %s\n", yytext); BEGIN(INITIAL); }
[ \t\r\n]+ ;
.           { printf("TOKEN : %s\n", yytext); BEGIN(INITIAL); }

%%

int yywrap()
{
    return 1;
}

int main()
{
    yylex();
    return 0;
}

%{
#include <stdio.h>
%}

%s LABEL
id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

goto        { puts("KEYWORD : goto"); BEGIN(LABEL); }
<LABEL>{id} { printf("LABEL REFERENCE : %s\n", yytext); BEGIN(INITIAL); }
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

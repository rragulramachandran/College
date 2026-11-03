%{
#include <stdio.h>
%}

%s TYPE
id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

int|char|float|double|void|long|short|signed|unsigned { printf("TYPE KEYWORD : %s\n", yytext); BEGIN(TYPE); }
<TYPE>"*"/{ws}{id} { puts("POINTER DECLARATOR : *"); BEGIN(INITIAL); }
"*" { puts("MULTIPLICATION OPERATOR : *"); BEGIN(INITIAL); }
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

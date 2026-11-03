%{
#include <stdio.h>
%}

%s TAG
id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

struct { puts("KEYWORD : struct"); BEGIN(TAG); }
<TAG>{id}/{ws}"{" { printf("STRUCT TAG (DEFINITION) : %s\n", yytext); BEGIN(INITIAL); }
<TAG>{id}/[ \t]+{id} { printf("STRUCT TAG (DECLARATION) : %s\n", yytext); BEGIN(INITIAL); }
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

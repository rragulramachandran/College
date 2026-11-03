%{
#include <stdio.h>
int depth = 0;
%}

%s EXPECT
id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

vector|list|map|set { printf("TEMPLATE IDENTIFIER : %s\n", yytext); BEGIN(EXPECT); }
<EXPECT>"<" { depth++; puts("TEMPLATE OPEN : <"); BEGIN(INITIAL); }
"<" { puts("RELATIONAL OPERATOR : <"); BEGIN(INITIAL); }
">" { if (depth > 0) { depth--; puts("TEMPLATE CLOSE : >"); } else puts("RELATIONAL OPERATOR : >"); BEGIN(INITIAL); }
"<="|">=" { printf("RELATIONAL OPERATOR : %s\n", yytext); BEGIN(INITIAL); }
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

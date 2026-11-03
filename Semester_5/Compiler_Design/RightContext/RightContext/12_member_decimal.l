%{
#include <stdio.h>
%}

%s AFTER_ID
id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

{id} { printf("IDENTIFIER : %s\n", yytext); BEGIN(AFTER_ID); }
<AFTER_ID>\.[0-9]* { yyless(1); puts("MEMBER ACCESS : ."); BEGIN(INITIAL); }
[0-9]+\.[0-9]*|\.[0-9]+ { printf("FLOAT : %s\n", yytext); BEGIN(INITIAL); }
[0-9]+ { printf("INTEGER : %s\n", yytext); BEGIN(INITIAL); }
[ \t\r\n]+ { BEGIN(INITIAL); }
. { printf("TOKEN : %s\n", yytext); BEGIN(INITIAL); }

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

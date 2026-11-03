%{
#include <stdio.h>
%}

%s COMMENT
id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

"/*"          { BEGIN(COMMENT); }
<COMMENT>"*/" { BEGIN(INITIAL); }
<COMMENT>[^*\n]+ ;
<COMMENT>"*" ;
<COMMENT>\n  ;
<COMMENT><<EOF>> { puts("ERROR : Unterminated comment"); return 0; }
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

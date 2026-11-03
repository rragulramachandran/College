%{
#include <stdio.h>
%}

%s STRING
id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

<INITIAL>\" { puts("STRING BEGIN"); BEGIN(STRING); }
<STRING>\" { puts("STRING END"); BEGIN(INITIAL); }
<STRING>\\. { printf("STRING ESCAPE : %s\n", yytext); }
<STRING>[^\\\"\n]+ { printf("STRING TEXT : %s\n", yytext); }
<STRING>\n { puts("ERROR : Newline in string"); BEGIN(INITIAL); }
<STRING>\\ { puts("ERROR : Incomplete escape"); }
<STRING><<EOF>> { puts("ERROR : Unterminated string"); return 0; }
if|else|while|int|return { printf("KEYWORD : %s\n", yytext); }
[+*/=<>!-] { printf("OPERATOR : %s\n", yytext); }
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

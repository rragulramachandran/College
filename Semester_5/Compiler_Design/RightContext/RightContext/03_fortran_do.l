%{
#include <stdio.h>
%}


id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

[dD][oO]{ws}[0-9]+{ws}{id}{ws}"="/{ws}[0-9]+{ws}"," { printf("DO LOOP : %s\n", yytext); }
[dD][oO]{ws}[0-9]+{ws}{id}{ws}"=" { int i; printf("ASSIGNMENT : "); for (i = 0; i < yyleng; i++) if (yytext[i] != ' ' && yytext[i] != '\t') putchar(yytext[i]); putchar('\n'); }
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

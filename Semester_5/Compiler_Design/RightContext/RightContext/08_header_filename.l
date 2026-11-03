%{
#include <stdio.h>
%}

%s HEADER
id      [a-zA-Z_][a-zA-Z0-9_]*
ws      [ \t]*

%%

#[ \t]*include { puts("DIRECTIVE : #include"); BEGIN(HEADER); }
<HEADER>\"[^\"\n]+\"|\<[^>\n]+\> { printf("HEADER FILENAME : %s\n", yytext); BEGIN(INITIAL); }
<HEADER>\n { BEGIN(INITIAL); }
\"([^\\\"\n]|\\.)*\" { printf("STRING LITERAL : %s\n", yytext); BEGIN(INITIAL); }
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

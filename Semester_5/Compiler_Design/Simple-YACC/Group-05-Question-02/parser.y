/* Switch with one or more cases
 * Sample input: switch ( id ) { case num : id = id + num ; break ; case num : id = num ; break ; }
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token ID NUM SWITCH CASE BREAK

%%
S : SWITCH '(' ID ')' '{' cases '}' ;
cases : cases case_body | case_body ;
case_body : CASE NUM ':' assignment ';' BREAK ';' ;
assignment : ID '=' expression ;
expression : expression '+' term | expression '-' term | term ;
term : term '*' factor | term '/' factor | factor ;
factor : ID | NUM | '(' expression ')' ;
%%

int main(void)
{
    if (yyparse() != 0) return 1;

    puts("Valid");
    return 0;
}

int yyerror(const char *s)
{
    (void)s;
    puts("Invalid");
    return 0;
}

/* Integer function definition with assignment body
 * Sample input: int id ( int id , int id ) { id = id + num ; }
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token ID NUM INT

%%
S : INT ID '(' parameters ')' '{' assignment ';' '}' ;
parameters : parameters ',' INT ID | INT ID ;
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

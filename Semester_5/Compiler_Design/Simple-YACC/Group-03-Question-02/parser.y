/* If-else with assignment in both branches
 * Sample input: if ( id < num ) { id = num ; } else { id = id - num ; }
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token ID NUM IF ELSE LE GE EQ NE

%%
S : IF '(' condition ')' '{' assignment ';' '}' ELSE '{' assignment ';' '}' ;
assignment : ID '=' expression ;
condition : expression relop expression ;
relop : '<' | '>' | LE | GE | EQ | NE ;
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

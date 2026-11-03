/* Relational expression
 * Sample input: id<num
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token ID NUM LE GE EQ NE

%%
S : condition ;
condition : expression relop expression ;
relop : '<' | '>' | LE | GE | EQ | NE ;
expression : expression '+' term | expression '-' term | term ;
term : term '*' factor | term '/' factor | factor ;
factor : ID | NUM | '(' expression ')' ;
%%

int main(void)
{
    if (yyparse() != 0) return 1;

    puts("Valid expression");
    return 0;
}

int yyerror(const char *s)
{
    (void)s;
    puts("Invalid expression");
    return 0;
}

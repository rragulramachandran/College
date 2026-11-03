/* For loop with assignment body
 * Sample input: for ( id = num ; id < num ; id = id + num ) { id = id + num ; }
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token ID NUM FOR LE GE EQ NE

%%
S : for_head '{' assignment ';' '}' ;
for_head : FOR '(' assignment ';' condition ';' assignment ')' ;
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

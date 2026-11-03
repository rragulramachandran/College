/* Java-style class with integer fields
 * Sample input: class id { int id ; int id ; }
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token ID NUM CLASS INT

%%
S : CLASS ID '{' members '}' ;
members : members INT ID ';' | INT ID ';' ;

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

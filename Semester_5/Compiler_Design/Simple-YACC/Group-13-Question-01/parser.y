/* Nested parentheses around an identifier
 * Sample input: (((id)))
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token ID

%%
S : '(' S ')' | ID ;
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

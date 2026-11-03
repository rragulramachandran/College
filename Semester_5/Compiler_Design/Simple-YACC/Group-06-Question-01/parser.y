/* Nonempty bracketed identifier list
 * Sample input: [id,id,id]
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
S : '[' list ']' ;
list : list ',' ID | ID ;
%%

int main(void)
{
    if (yyparse() != 0) return 1;

    puts("Valid string");
    return 0;
}

int yyerror(const char *s)
{
    (void)s;
    puts("Invalid string");
    return 0;
}

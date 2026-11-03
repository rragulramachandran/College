/* Boolean expressions: not, and, or (in descending precedence)
 * Sample input: true and ( false or true )
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token OR AND NOT TRUE FALSE

%%
E : E OR T | T ;
T : T AND F | F ;
F : NOT F | '(' E ')' | TRUE | FALSE ;
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

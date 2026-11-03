/* L = { (ab)^n | n >= 1 }
 * Sample input: ababab
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}


%%
S : S 'a' 'b' | 'a' 'b' ;
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

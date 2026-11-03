/* L = { a^n b^n c^m | n,m >= 1 }
 * Sample input: aabbcc
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}


%%
S : matched tail ;
matched : 'a' matched 'b' | 'a' 'b' ;
tail : tail 'c' | 'c' ;
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

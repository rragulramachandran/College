/* Even-length palindromes over {a,b}, including the empty string.
 * The grammar P -> aPa | bPb | empty is not LALR(1).
 * Collect characters with YACC, then check symmetry in C.
 * Sample input: abba
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);
static char text[10000];
static int length = 0;
%}

%token INVALID

%%
S : S 'a' { text[length++] = 'a'; }
  | S 'b' { text[length++] = 'b'; }
  | /* empty */ ;
%%

int main(void)
{
    if (yyparse() != 0) return 1;
    int i;
    for (i = 0; i < length / 2; ++i)
        if (text[i] != text[length - 1 - i]) { puts("Invalid string"); return 1; }
    if (length % 2) { puts("Invalid string"); return 1; }
    puts("Valid string");
    return 0;
}

int yyerror(const char *s)
{
    (void)s;
    puts("Invalid string");
    return 0;
}

/* Dangling else: else binds to the closest unmatched if
 * Sample input: if id then id = id ; else id = num ;
 * Build: yacc -d parser.y && lex parser.l && cc y.tab.c lex.yy.c -o parser
 * Run: ./parser < input.txt
 */
%{
#include <stdio.h>
int yylex(void);
int yyerror(const char *s);

%}

%token ID NUM IF THEN ELSE
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
S : statement ;
statement : IF ID THEN statement %prec LOWER_THAN_ELSE
          | IF ID THEN statement ELSE statement
          | assignment ';' ;
assignment : ID '=' expression ;
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

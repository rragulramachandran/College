%{
#include <stdio.h>
int result;
int yylex(void);
int yyerror(const char *s);
%}

%token NUM
%left '+' '-'
%left '*' '/'
%right UMINUS

%%

start : expr { result = $1; }
      ;

expr : NUM                   { $$ = $1; }
     | expr '+' expr         { $$ = $1 + $3; }
     | expr '-' expr         { $$ = $1 - $3; }
     | expr '*' expr         { $$ = $1 * $3; }
     | expr '/' expr         { $$ = $1 / $3; }
     | '-' expr %prec UMINUS { $$ = -$2; }
     | '(' expr ')'          { $$ = $2; }
     ;
%%


int main(void)
{
    printf("Enter expression: ");
    if (yyparse() == 0)
        printf("Valid expression\n");
    return 0;
}

int yyerror(const char *s)
{
    printf("Invalid expression\n");
    return 0;
}
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXQ 100

int yylex(void);
void yyerror(const char *s);

struct Quadruple {
    char op[20];
    char arg1[20];
    char arg2[20];
    char result[20];
} quad[MAXQ];

int qcount = 0;
int tcount = 0;
int lcount = 0;

void addquad(const char *op, const char *a1, const char *a2, const char *res)
{
    if (qcount >= MAXQ) {
        fprintf(stderr, "Quadruple table overflow!\n");
        exit(1);
    }
    strncpy(quad[qcount].op,     op,  sizeof(quad[qcount].op)     - 1);
    strncpy(quad[qcount].arg1,   a1,  sizeof(quad[qcount].arg1)   - 1);
    strncpy(quad[qcount].arg2,   a2,  sizeof(quad[qcount].arg2)   - 1);
    strncpy(quad[qcount].result, res, sizeof(quad[qcount].result)  - 1);
    qcount++;
}

char *newtemp(void)
{
    char *s = (char *)malloc(20);
    sprintf(s, "t%d", ++tcount);
    return s;
}

char *newlabel(void)
{
    char *s = (char *)malloc(20);
    sprintf(s, "L%d", ++lcount);
    return s;
}

void printquads(void)
{
    int i;
    printf("\n%-12s %-8s %-8s %-8s\n", "Op", "Arg1", "Arg2", "Result");
    printf("%-12s %-8s %-8s %-8s\n", "----------", "------", "------", "------");
    for (i = 0; i < qcount; i++) {
        printf("%-12s %-8s %-8s %-8s\n",
               quad[i].op, quad[i].arg1, quad[i].arg2, quad[i].result);
    }
    printf("\n");
}
%}

%union {
    char *str;
}

%token <str> ID NUM RELOP
%token IF WHILE

%type <str> expr cond stmt assign if_stmt while_stmt

%left '+' '-'
%left '*' '/'

%%

program
    : stmts                     { printquads(); }
    ;

stmts
    : stmts stmt
    |
    ;

stmt
    : assign                    { $$ = $1; }
    | if_stmt                   { $$ = $1; }
    | while_stmt                { $$ = $1; }
    ;

assign
    : ID '=' expr ';'           { addquad("=", $3, "-", $1);
                                  $$ = $1; }
    ;

if_stmt
    : IF '(' cond ')'
        {
            $<str>$ = newlabel();
            addquad("if_false", $3, "-", $<str>$);
        }
      '{' stmts '}'
        {
            addquad("label", "-", "-", $<str>5);
            $$ = $<str>5;
        }
    ;

while_stmt
    : WHILE
        {
            $<str>$ = newlabel();
            addquad("label", "-", "-", $<str>$);
        }
      '(' cond ')'
        {
            $<str>$ = newlabel();
            addquad("if_false", $4, "-", $<str>$);
        }
      '{' stmts '}'
        {
            addquad("goto", "-", "-", $<str>2);
            addquad("label", "-", "-", $<str>6);
            $$ = $<str>6;
        }
    ;

cond
    : expr RELOP expr           { $$ = newtemp();
                                  addquad($2, $1, $3, $$); }
    ;

expr
    : expr '+' expr             { $$ = newtemp();
                                  addquad("+", $1, $3, $$); }
    | expr '-' expr             { $$ = newtemp();
                                  addquad("-", $1, $3, $$); }
    | expr '*' expr             { $$ = newtemp();
                                  addquad("*", $1, $3, $$); }
    | expr '/' expr             { $$ = newtemp();
                                  addquad("/", $1, $3, $$); }
    | '(' expr ')'              { $$ = $2; }
    | ID                        { $$ = $1; }
    | NUM                       { $$ = $1; }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Parse error: %s\n", s);
}

int main(void)
{
    printf("Enter statements, then signal EOF (Ctrl+D on Unix):\n");
    return yyparse();
}
%{
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int yylex(void);
void yyerror(const char *s);


%}

%%


S   :   'a' A 'c'
    |   'a' S 'c'
    |   'a' 'c'
    ;
A   :    A 'b'
    |   'b'
    ;

%%

int main(){
    printf("enter the string:");

    if(yyparse()==0){
        printf("valid string\n");
    }
    return 0;
}

void yyerror(const char *s){
    printf("invalid string");
}
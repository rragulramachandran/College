%{
#include<stdio.h>
#include "power.tab.h"

%}

%%

a   {return 'a';}
b   {return 'b';}
c   {return 'c';}

[ \t\n] ;

.   {return yytext[0];}

%%

int yywrap()
{
    return 1;
}
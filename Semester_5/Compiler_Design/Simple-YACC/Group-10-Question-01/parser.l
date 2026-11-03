%{
#include "y.tab.h"
%}

%%
"or" { return OR; }
"and" { return AND; }
"not" { return NOT; }
"true" { return TRUE; }
"false" { return FALSE; }
[ \t\r\n]+ ;
. { return (unsigned char)yytext[0]; }
%%

int yywrap(void)
{
    return 1;
}

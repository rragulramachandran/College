%{
#include "y.tab.h"
%}

%%
"id" { return ID; }
[ \t\r\n]+ ;
. { return (unsigned char)yytext[0]; }
%%

int yywrap(void)
{
    return 1;
}

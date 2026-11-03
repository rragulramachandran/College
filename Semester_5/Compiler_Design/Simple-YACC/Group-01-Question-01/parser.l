%{
#include "y.tab.h"
%}

%%

[ \t\r\n]+ ;
. { return (unsigned char)yytext[0]; }
%%

int yywrap(void)
{
    return 1;
}

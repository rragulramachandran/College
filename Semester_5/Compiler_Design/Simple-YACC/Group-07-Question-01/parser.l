%{
#include "y.tab.h"
static int length_guard = 0;
%}

%%

[ \t\r\n]+ ;
[ab] { if (length_guard++ >= 9999) return INVALID; return yytext[0]; }
. { return (unsigned char)yytext[0]; }
%%

int yywrap(void)
{
    return 1;
}

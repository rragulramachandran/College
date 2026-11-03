%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
int line_seen = 0;
int is_keyword(const char *s)
{
    const char *keywords[] = {
        "auto",   "break",  "case",     "char",   "const",    "continue", "default",  "do",
        "double", "else",   "enum",     "extern", "float",    "for",      "goto",     "if",
        "int",    "long",   "register", "return", "short",    "signed",   "sizeof",   "static",
        "struct", "switch", "typedef",  "union",  "unsigned", "void",     "volatile", "while"};
    int i;
    for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        if (strcmp(s, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}
%}

%%

[^\n]+    {
    line_seen = 1;
    int valid = (isalpha((unsigned char)yytext[0]) || yytext[0] == '_');
    int i;
    for (i = 1; i < yyleng; i++)
    {
        if (!isalnum((unsigned char)yytext[i]) && yytext[i] != '_')
        {
            valid = 0;
        }
    }
    if (is_keyword(yytext))
    {
        valid = 0;
    }
    puts(valid ? "Valid C identifier" : "Invalid C identifier");
}

\n    {
    if (!line_seen)
    {
        puts("Invalid C identifier");
    }
    line_seen = 0;
}

%%

int main()
{
    yylex();
    return 0;
}

int yywrap()
{

    return 1;
}

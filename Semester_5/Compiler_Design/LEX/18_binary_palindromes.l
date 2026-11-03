%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE *output;
int count = 0;
void initialize_output_files(void)
{
    output = fopen("binary_palindromes.txt", "w");
    if (!output)
    {
        perror("Output file");
        exit(1);
    }
}

#define YY_USER_INIT initialize_output_files();
%}

%%

[^ \t\r\n\f\v]+    {
    int i, valid = 1;
    for (i = 0; i < yyleng; i++)
    {
        if (yytext[i] != '0' && yytext[i] != '1')
        {
            valid = 0;
        }
    }
    for (i = 0; i < yyleng / 2; i++)
    {
        if (yytext[i] != yytext[yyleng - 1 - i])
        {
            valid = 0;
        }
    }
    if (valid)
    {
        count++;
        fprintf(output, "%s\n", yytext);
    }
}

[ \t\r\n\f\v]+    {
    /* Ignore this match. */
}

%%

int main()
{
    yylex();
    return 0;
}

int yywrap()
{
    printf("Palindrome count: %d\n", count);
    fclose(output);
    return 1;
}

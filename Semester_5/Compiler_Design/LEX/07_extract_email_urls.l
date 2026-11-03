%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE *emails, *urls;
void initialize_output_files(void)
{
    emails = fopen("emails.txt", "w");
    urls = fopen("urls.txt", "w");
    if (!emails || !urls)
    {
        perror("Output file");
        exit(1);
    }
}

#define YY_USER_INIT initialize_output_files();
%}

EMAIL [a-zA-Z0-9_%+-]+(\.[a-zA-Z0-9_%+-]+)*@[a-zA-Z0-9]+([.-][a-zA-Z0-9]+)*\.[a-zA-Z]{2,}
URL (https?:\/\/|ftp:\/\/|www\.)[a-zA-Z0-9]([a-zA-Z0-9.-]*[a-zA-Z0-9])?(:[0-9]+)?([/?#][^ \t\r\n<>\"\']*)?

%%

{EMAIL}    {
    fprintf(emails, "%s\n", yytext);
}

{URL}    {
    int n = yyleng;
    while (n && strchr(".,;:!?)]}", yytext[n - 1]))
    {
        n--;
    }
    fprintf(urls, "%.*s\n", n, yytext);
}

.|\n    {
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
    fclose(emails);
    fclose(urls);
    return 1;
}

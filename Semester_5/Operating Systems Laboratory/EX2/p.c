#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

int countVowels(const char *str)
{
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        char ch = tolower(str[i]);
        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
            count++;
    }
    return count;
}

int isPalindrome(const char *str)
{
    int len = strlen(str);
    if (len == 0) return 1;

    int end = len - 1;
    while (end >= 0 && (str[end] == '\n' || str[end] == '\r' || str[end] == ' '))
        end--;
    int start = 0;
    while (start < end)
    {
        if (tolower(str[start]) != tolower(str[end]))
            return 0;
        start++;
        end--;
    }
    return 1;
}

int main()
{
    int pipe1[2];
    int pipe2[2];
    char str[81];

    if (pipe(pipe1) == -1)
    {
        perror("pipe1 failed");
        exit(1);
    }
    if (pipe(pipe2) == -1)
    {
        perror("pipe2 failed");
        exit(1);
    }

    printf("[P1] Process PID: %d\n", getpid());
    printf("[P1] Enter a string (max 80 chars): ");
    if (fgets(str, sizeof(str), stdin) == NULL)
    {
        close(pipe1[1]);
        return 0;
    }
    str[strcspn(str, "\r\n")] = '\0';
    printf("[P1] Writing string to pipe1: \"%s\"\n", str);
    write(pipe1[1], str, strlen(str) + 1);
    close(pipe1[1]);

    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork failed");
        exit(1);
    }

    if (pid2 == 0)
    {
        close(pipe1[1]);
        close(pipe2[0]);

        if (read(pipe1[0], str, sizeof(str)) > 0)
        {
            close(pipe1[0]);
            printf("[P2] Process PID: %d\n", getpid());
            printf("[P2] Read string from pipe1: \"%s\"\n", str);
            int vowels = countVowels(str);
            printf("[P2] Number of vowels in string: %d\n", vowels);
            write(pipe2[1], str, strlen(str) + 1);
            close(pipe2[1]);

            pid_t pid3 = fork();
            if (pid3 < 0)
            {
                perror("fork failed");
                exit(1);
            }

            if (pid3 == 0)
            {
                close(pipe2[1]);
                if (read(pipe2[0], str, sizeof(str)) > 0)
                {
                    close(pipe2[0]);
                    printf("[P3] Process PID: %d\n", getpid());
                    printf("[P3] Read string from pipe2: \"%s\"\n", str);
                    if (isPalindrome(str))
                        printf("[P3] The string \"%s\" is a Palindrome.\n", str);
                    else
                        printf("[P3] The string \"%s\" is NOT a Palindrome.\n", str);
                }
                exit(0);
            }
            else
            {
                wait(NULL);
            }
        }
        exit(0);
    }
    else
    {
        wait(NULL);
        printf("[P1] Process tree execution completed.\n");
    }

    return 0;
}

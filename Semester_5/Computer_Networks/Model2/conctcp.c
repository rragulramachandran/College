
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    int s, c, n, i;
    char roll[20], result[100];
    struct sockaddr_in addr, client;
    socklen_t len;

    char *rno[] = {"101", "102", "103"};
    char *name[] = {"Arun", "Bala", "Kumar"};
    char *dept[] = {"CSE", "ECE", "IT"};

    signal(SIGCHLD, SIG_IGN);

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    listen(s, 5);

    printf("Concurrent TCP Server waiting...\n");

    while (1)
    {
        len = sizeof(client);
        c = accept(s, (struct sockaddr *)&client, &len);

        if (c < 0)
            continue;

        if (fork() == 0)
        {
            close(s);

            n = recv(c, roll, sizeof(roll) - 1, 0);

            if (n > 0)
            {
                roll[n] = '\0';

                for (i = 0; i < 3; i++)
                {
                    if (strcmp(roll, rno[i]) == 0)
                        break;
                }

                if (i < 3)
                    sprintf(result, "Name: %s, Department: %s",
                            name[i], dept[i]);
                else
                    strcpy(result, "Student not found");

                send(c, result, strlen(result), 0);

                printf("Request: %s\n", roll);
            }

            close(c);
            return 0;
        }

        close(c);
    }

    close(s);
    return 0;
}

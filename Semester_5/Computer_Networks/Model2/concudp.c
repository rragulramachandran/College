
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    int s, n, i;
    char roll[20], result[100];
    struct sockaddr_in addr, client;
    socklen_t len;

    char *rno[] = {"101", "102", "103"};
    char *name[] = {"Arun", "Bala", "Kumar"};
    char *dept[] = {"CSE", "ECE", "IT"};

    signal(SIGCHLD, SIG_IGN);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    printf("Concurrent UDP Server waiting...\n");

    while (1)
    {
        len = sizeof(client);

        n = recvfrom(s, roll, sizeof(roll) - 1, 0,
                     (struct sockaddr *)&client, &len);

        if (n < 0)
            continue;

        roll[n] = '\0';

        if (fork() == 0)
        {
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

            sendto(s, result, strlen(result), 0,
                   (struct sockaddr *)&client, len);

            close(s);
            return 0;
        }
    }

    close(s);
    return 0;
}

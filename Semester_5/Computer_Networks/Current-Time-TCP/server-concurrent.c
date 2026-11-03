
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>

int main()
{
    int s, c;
    struct sockaddr_in server, client;
    char msg[100];
    socklen_t clientLen = sizeof(client);

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);

    while (1)
    {
        clientLen = sizeof(client);
        c = accept(s, (struct sockaddr *)&client, &clientLen);

        if (c < 0)
            continue;

        if (fork() == 0)
        {
            close(s);

            int n = recv(c, msg, sizeof(msg) - 1, 0);

            if (n > 0)
            {
                msg[n] = '\0';
                printf("Client Request : %s\n", msg);

                time_t now = time(NULL);
                strcpy(msg, ctime(&now));

                printf("Sending to Client : %s", msg);
                send(c, msg, strlen(msg) + 1, 0);
            }

            close(c);
            exit(0);
        }

        close(c);
    }

    close(s);
    return 0;
}

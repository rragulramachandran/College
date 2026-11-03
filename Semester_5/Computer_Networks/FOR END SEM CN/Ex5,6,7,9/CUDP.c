Server / Receiver
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int s;
    int client_no = 1;
    char msg[100];

    s = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));

    while(1)
    {
        /* Client to Server */
        recvfrom(s, msg, sizeof(msg), 0,
                 (struct sockaddr*)&client, &len);

        if(fork() == 0)
        {
            /* Display Client message */
            printf("Client %d: %s", client_no, msg);

            /* Server to Client */
            printf("Server: ");
            fgets(msg, sizeof(msg), stdin);

            sendto(s, msg, strlen(msg), 0,
                   (struct sockaddr*)&client, len);

            return 0;
        }

        client_no++;
    }

    return 0;
}
Client / Sender
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int s;
    char msg[100];

    s = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);

    /* Client to Server */
    printf("Client: ");
    fgets(msg, sizeof(msg), stdin);

    sendto(s, msg, strlen(msg), 0,
           (struct sockaddr*)&server, len);

    /* Server to Client */
    recvfrom(s, msg, sizeof(msg), 0,
             (struct sockaddr*)&server, &len);

    printf("Server: %s", msg);

    close(s);

    return 0;
}
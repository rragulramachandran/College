Server / Receiver
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int s, c;
    char msg[100];

    s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));
    listen(s, 5);

    c = accept(s, NULL, NULL);

    /* Client to Server */
    recv(c, msg, sizeof(msg), 0);
    printf("Client: %s", msg);

    /* Server to Client */
    printf("Server: ");
    fgets(msg, sizeof(msg), stdin);

    send(c, msg, strlen(msg), 0);

    close(c);
    close(s);

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

    s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);

    connect(s, (struct sockaddr*)&server, sizeof(server));

    /* Client to Server */
    printf("Client: ");
    fgets(msg, sizeof(msg), stdin);

    send(s, msg, strlen(msg), 0);

    /* Server to Client */
    recv(s, msg, sizeof(msg), 0);
    printf("Server: %s", msg);

    close(s);

    return 0;
}
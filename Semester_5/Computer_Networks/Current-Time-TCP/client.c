#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

int main()
{
    int s;
    struct sockaddr_in server;
    char msg[100];

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("172.16.16.200");

    int valid = connect(s, (struct sockaddr *)&server, sizeof(server));

    if (valid < 0)
    {
        close(s);
        return 1;
    }

    strcpy(msg, "TIME");
    send(s, msg, strlen(msg) + 1, 0);

    int n = recv(s, msg, sizeof(msg) - 1, 0);

    if (n > 0)
    {
        msg[n] = '\0';
        printf("Current Server Time : %s", msg);
    }

    close(s);
    return 0;
}

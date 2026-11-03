#include<stdio.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>

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

    while(1)
    {
        c = accept(s, (struct sockaddr *)&client, &clientLen);

        if(fork() == 0)
        {
            close(s);
            recv(c, &msg, sizeof(msg), 0);
            printf("Sending to Client : %s\n", msg);
            send(c, &msg, sizeof(msg), 0);
            close(c);
        }

        close(c);
    }

    close(s);
    return 0;

}
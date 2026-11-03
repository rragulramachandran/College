#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server = {0}, client = {0};

    int msg[100];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(server_fd, (struct sockaddr *)&server, sizeof(server));

    listen(server_fd, 5);

    while(1)
    {
        socklen_t client_len = sizeof(client);
        client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

        recv(client_fd, msg, sizeof(msg), 0);

        int str = msg[0];

        send(client_fd, &str, sizeof(str), 0);

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
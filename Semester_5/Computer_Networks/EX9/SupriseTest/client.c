#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main()
{
    int server_fd;
    struct sockaddr_in server = {0};
    int msg = 1;
    int str;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(server_fd, (struct sockaddr *)&server, sizeof(server));

    send(server_fd, &msg, sizeof(msg), 0);
    recv(server_fd, &str, sizeof(str), 0);

    printf("%d", str);
    close(server_fd);
    return 0;
}
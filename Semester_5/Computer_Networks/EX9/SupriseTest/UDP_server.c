#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    int server_fd;
    struct sockaddr_in server = {0}, client = {0};

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_fd, (struct sockaddr *)&server,
             sizeof(server)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    while (1) {
        int msg;
        socklen_t client_len = sizeof(client);

        ssize_t n = recvfrom(server_fd, &msg, sizeof(msg), 0,
                             (struct sockaddr *)&client, &client_len);
        if (n == -1) {
            perror("recvfrom");
            continue;
        }
        if (n != sizeof(msg))
            continue;

        if (sendto(server_fd, &msg, sizeof(msg), 0,
                   (struct sockaddr *)&client, client_len) == -1) {
            perror("sendto");
        }
    }

    close(server_fd);
    return 0;
}l
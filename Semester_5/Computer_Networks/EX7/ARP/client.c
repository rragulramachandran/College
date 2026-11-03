#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in saddr;
    char ip[1024];
    char buf[1024];

    if (argc != 3)
    {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return 1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &saddr.sin_addr) <= 0)
    {
        printf("Invalid server IP address.\n");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("connect");
        close(sock);
        return 1;
    }

    printf("Connected to ARP server.\n");
    printf("Enter IP address: ");
    fflush(stdout);
    fgets(ip, sizeof(ip), stdin);
    ip[strcspn(ip, "\r\n")] = '\0';
    send(sock, ip, strlen(ip) + 1, 0);
    int n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n > 0)
    {
        buf[n] = '\0';
        printf("\nServer Reply:\n%s", buf);
    }
    else if (n == 0)
    {
        printf("\nServer closed the connection prematurely.\n");
    }
    else
    {
        perror("recv failed");
    }

    close(sock);
    return 0;
}

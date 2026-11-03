#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 6001
#define MAXLINE 1024

int main(void)
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr = {0};

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int n;
    socklen_t len = sizeof(servaddr);

    printf("UDP Chat Client Started. Type 'exit' to stop.\n");

    while (1)
    {

        printf("Client (You): ");
        memset(buffer, 0, MAXLINE);
        fgets(buffer, MAXLINE, stdin);

        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, len);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Exiting chat...\n");
            break;
        }

        n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&servaddr, &len);
        if (n < 0)
        {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';
        printf("Server: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Server closed the chat.\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}

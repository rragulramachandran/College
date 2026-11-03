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
    struct sockaddr_in servaddr = {0}, cliaddr = {0};

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Chat Server is running on port %d...\n", PORT);

    int n;
    socklen_t len = sizeof(cliaddr);

    while (1)
    {

        n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0)
        {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';
        printf("\nClient: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        printf("Server (You): ");
        memset(buffer, 0, MAXLINE);
        fgets(buffer, MAXLINE, stdin);

        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Server exiting...\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}

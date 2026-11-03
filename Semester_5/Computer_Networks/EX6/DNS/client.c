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

    printf("DNS Client Started\nType 'exit' to stop.\n\n");

    while (1)
    {

        printf("Enter domain name: ");
        memset(buffer, 0, MAXLINE);

        fgets(buffer, MAXLINE, stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, len);

        if (strcmp(buffer, "exit") == 0)
        {
            printf("Client exiting...\n");
            break;
        }

        n = recvfrom(sockfd, buffer, MAXLINE - 1, 0, (struct sockaddr *)&servaddr, &len);

        if (n < 0)
        {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0';

        printf("IP Address: %s\n\n", buffer);
    }

    close(sockfd);

    return 0;
}

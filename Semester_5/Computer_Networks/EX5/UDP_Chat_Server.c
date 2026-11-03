#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 6001
#define MAXLINE 1024

int main()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // 1. Create UDP socket (SOCK_DGRAM)
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // 2. Bind the socket to the port
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

    // 3. Iterative Loop: Wait, Receive, Process, Reply
    while (1)
    {
        // Step A: Wait for message from client
        memset(buffer, 0, MAXLINE);
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
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

        // Step B: Get server's reply from terminal
        printf("Server (You): ");
        memset(buffer, 0, MAXLINE);
        fgets(buffer, MAXLINE, stdin);

        // Step C: Send reply directly back to the client that just sent the message
        sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);
        
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Server exiting...\n");
            break;
        }
    }

    // 4. Close socket
    close(sockfd);
    return 0;
}

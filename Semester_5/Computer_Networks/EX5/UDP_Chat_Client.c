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
    struct sockaddr_in servaddr;

    // 1. Create UDP socket (SOCK_DGRAM)
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // 2. Set Server address information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    
    // FIX: Client must explicitly send to 127.0.0.1 (localhost).
    // Sending to INADDR_ANY (0.0.0.0) from a client can cause routing failures on macOS/Unix.
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int n;
    socklen_t len = sizeof(servaddr);

    printf("UDP Chat Client Started. Type 'exit' to stop.\n");

    // 3. Iterative Loop: Send, Wait, Receive
    while (1)
    {
        // Step A: Get client's message from terminal
        printf("Client (You): ");
        memset(buffer, 0, MAXLINE);
        fgets(buffer, MAXLINE, stdin);

        // Step B: Send message to server
        sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, len);
        
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Exiting chat...\n");
            break;
        }

        // Step C: Wait for server's reply
        memset(buffer, 0, MAXLINE);
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&servaddr, &len);
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

    // 4. Close socket
    close(sockfd);
    return 0;
}

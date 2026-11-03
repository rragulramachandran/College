#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 6001
#define MAXLINE 1024
#define MAX_RECORDS 100

struct DNSRecord
{
    char domain[100];
    char ip[INET_ADDRSTRLEN];
};

static void reply(int sockfd, const char *text, const struct sockaddr_in *client, socklen_t len)
{
    sendto(sockfd, text, strlen(text), 0, (const struct sockaddr *)client, len);
}

int main(void)
{
    int sockfd;
    char buffer[MAXLINE];

    struct sockaddr_in servaddr = {0}, cliaddr = {0};

    struct DNSRecord table[MAX_RECORDS];
    int recordCount = 0;

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

    printf("DNS Server running on port %d...\n\n", PORT);

    socklen_t len = sizeof(cliaddr);

    while (1)
    {
        int n;
        int found = 0;

        n = recvfrom(sockfd, buffer, MAXLINE - 1, 0, (struct sockaddr *)&cliaddr, &len);

        if (n < 0)
        {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0';

        if (strcmp(buffer, "exit") == 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        printf("Client requested: %s\n", buffer);

        for (int i = 0; i < recordCount; i++)
        {
            if (strcmp(table[i].domain, buffer) == 0)
            {
                found = 1;

                printf("Found in local DNS table.\n");
                printf("%s -> %s\n\n", table[i].domain, table[i].ip);

                reply(sockfd, table[i].ip, &cliaddr, len);

                break;
            }
        }

        if (!found)
        {
            struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_DGRAM};
            struct addrinfo *result;

            int status = getaddrinfo(buffer, NULL, &hints, &result);

            if (status != 0)
            {
                printf("Unable to resolve domain: %s\n\n", buffer);

                reply(sockfd, "Domain not found", &cliaddr, len);

                continue;
            }

            struct sockaddr_in *address;
            char ip[INET_ADDRSTRLEN];

            address = (struct sockaddr_in *)result->ai_addr;

            inet_ntop(AF_INET, &(address->sin_addr), ip, sizeof(ip));

            printf("Resolved using DNS.\n");
            printf("%s -> %s\n", buffer, ip);

            if (recordCount < MAX_RECORDS)
            {
                strcpy(table[recordCount].domain, buffer);
                strcpy(table[recordCount].ip, ip);

                recordCount++;

                printf("Stored in local DNS table.\n\n");
            }

            reply(sockfd, ip, &cliaddr, len);

            freeaddrinfo(result);
        }
    }

    close(sockfd);

    return 0;
}

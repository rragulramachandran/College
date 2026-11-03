#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define SERVER_IP "127.0.0.1"

#define REQ_SETUP 1
#define REQ_ALLOCATE 2
#define REQ_EXIT 3

struct Message
{
    int type, total_ips, num_blocks, block_id, request_ips, status;
    char payload[1024];
};

static int exchange(int sockfd, struct sockaddr_in *server, socklen_t *len,
                    const struct Message *msg, struct Message *response)
{
    sendto(sockfd, msg, sizeof(*msg), 0, (struct sockaddr *)server, *len);
    memset(response, 0, sizeof(*response));
    return recvfrom(sockfd, response, sizeof(*response), 0, (struct sockaddr *)server, len);
}

int main(void)
{
    int sockfd;
    struct sockaddr_in server_addr = {0};
    socklen_t server_len = sizeof(server_addr);
    struct Message msg, response;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    printf("=== Custom DHCP Client ===\n");

    memset(&msg, 0, sizeof(msg));
    msg.type = REQ_SETUP;

    printf("Enter Total number of IP addresses required: ");
    scanf("%d", &msg.total_ips);

    printf("Enter Number of blocks/subnets required: ");
    scanf("%d", &msg.num_blocks);

    int n = exchange(sockfd, &server_addr, &server_len, &msg, &response);
    if (n > 0)
    {
        printf("\n--- Server Setup Response ---\n%s-----------------------------\n",
               response.payload);
    }
    else
    {
        perror("Failed to receive setup response from server");
        close(sockfd);
        exit(1);
    }

    if (response.status == 0)
    {
        close(sockfd);
        exit(1);
    }

    while (1)
    {
        memset(&msg, 0, sizeof(msg));

        printf("\nEnter which block to use (1 to %d, or 0 to EXIT): ",
               response.num_blocks > 0 ? response.num_blocks : 64);
        scanf("%d", &msg.block_id);

        if (msg.block_id == 0)
        {
            msg.type = REQ_EXIT;
            exchange(sockfd, &server_addr, &server_len, &msg, &response);

            printf("\n%s", response.payload);
            break;
        }

        msg.type = REQ_ALLOCATE;
        printf("Enter how many IP addresses to allocate: ");
        scanf("%d", &msg.request_ips);

        n = exchange(sockfd, &server_addr, &server_len, &msg, &response);
        if (n > 0)
        {
            printf("\n--- Allocation Response ---\n%s---------------------------\n",
                   response.payload);
        }
        else
        {
            perror("Failed to receive allocation response");
        }
    }

    close(sockfd);
    return 0;
}

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int is_quit(const char *message)
{
    return strcmp(message, "quit") == 0 || strcmp(message, "/quit") == 0;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in saddr;
    char buf[1024];

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);

    if (port <= 0 || port > 65535)
    {
        fprintf(stderr, "Invalid port number: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &saddr.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid server IP address: %s\n", argv[1]);
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("connect failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to chat server at %s:%d\n", argv[1], port);
    printf("Type 'quit' to end the chat.\n\n");
    int n = recv(sock, buf, sizeof(buf) - 1, 0);

    if (n > 0)
    {
        buf[n] = '\0';
        printf("%s\n\n", buf);
    }

    while (1)
    {
        printf("You: ");

        if (fgets(buf, sizeof(buf), stdin) == NULL)
        {
            strcpy(buf, "quit");
        }

        buf[strcspn(buf, "\n")] = '\0';

        if (send(sock, buf, strlen(buf), 0) < 0)
        {
            perror("send failed");
            break;
        }

        if (is_quit(buf))
        {
            printf("You ended the chat.\n");
            break;
        }

        n = recv(sock, buf, sizeof(buf) - 1, 0);

        if (n <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }

        buf[n] = '\0';
        printf("%s\n\n", buf);

        if (is_quit(buf))
        {
            printf("Server ended the chat.\n");
            break;
        }
    }

    close(sock);
    return 0;
}

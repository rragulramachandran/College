#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main()
{
    WSADATA wsa;
    int sockfd, choice, n;
    char word[5];
    char buffer[1024];

    struct sockaddr_in serverAddr;
    int addrLen = sizeof(serverAddr);

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == INVALID_SOCKET)
    {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    while (1)
    {
        printf("\n1. Send 4-character word\n");
        printf("2. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 2)
            break;

        if (choice == 1)
        {
            printf("Enter 4-character hexadecimal word: ");
            scanf("%4s", word);
            getchar();

            if (strlen(word) != 4)
            {
                printf("Enter exactly 4 characters\n");
                continue;
            }

            sendto(sockfd, word, strlen(word), 0,
                   (struct sockaddr *)&serverAddr,
                   sizeof(serverAddr));

            n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&serverAddr,
                         &addrLen);

            if (n == SOCKET_ERROR)
            {
                printf("Receive failed\n");
                break;
            }

            buffer[n] = '\0';

            printf("Bit-stuffed binary: %s\n", buffer);
        }
        else
        {
            printf("Invalid choice\n");
        }
    }

    closesocket(sockfd);
    WSACleanup();

    return 0;
}
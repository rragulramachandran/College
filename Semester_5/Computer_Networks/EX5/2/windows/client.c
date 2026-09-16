#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
int main()
{
    WSADATA wsa;
    int sockfd, choice, n;
    char message[1024], buffer[1024];
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
        printf("\n1. Send String\n");
        printf("2. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 2)
            break;

        if (choice == 1)
        {
            printf("Enter string: ");
            fgets(message, sizeof(message), stdin);

            message[strcspn(message, "\n")] = '\0';

            sendto(sockfd, message, strlen(message), 0,
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

            printf("Reversed String: %s\n", buffer);
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
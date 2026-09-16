#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
int main()
{
    WSADATA wsa;
    int sockfd, n;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    WSAStartup(MAKEWORD(2, 2), &wsa);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *)&serverAddr,sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("Bind failed\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("UDP Echo Server is running...\n");
    while (1)
    {
        n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,(struct sockaddr *)&clientAddr,&addrLen);
        if (n == SOCKET_ERROR)
        {
            printf("Receive failed\n");
            break;
        }
        buffer[n] = '\0';
        printf("Client: %s\n", buffer);
        sendto(sockfd, buffer, n, 0,(struct sockaddr *)&clientAddr,addrLen);
        printf("Echo sent\n");
    }
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
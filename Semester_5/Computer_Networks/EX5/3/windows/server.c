#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main()
{
    WSADATA wsa;
    int sockfd, n, i, j;
    char word[5];
    char binary[17];
    char stuffed[1024];
    int count;
    char temp;

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

    if (bind(sockfd, (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("Bind failed\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("UDP Bit Stuffing Server is running...\n");

    while (1)
    {
        n = recvfrom(sockfd, word, sizeof(word) - 1, 0,
                     (struct sockaddr *)&clientAddr,
                     &addrLen);

        if (n == SOCKET_ERROR)
        {
            printf("Receive failed\n");
            break;
        }

        word[n] = '\0';

        printf("Client: %s\n", word);

        /* Convert each hexadecimal character to 4 bits */
        for (i = 0; i < 4; i++)
        {
            int value;

            if (word[i] >= '0' && word[i] <= '9')
                value = word[i] - '0';
            else if (word[i] >= 'A' && word[i] <= 'F')
                value = word[i] - 'A' + 10;
            else
                value = word[i] - 'a' + 10;

            for (j = 3; j >= 0; j--)
            {
                binary[i * 4 + (3 - j)] = ((value >> j) & 1) + '0';
            }
        }

        binary[16] = '\0';

        printf("16-bit binary: %s\n", binary);

        /* Bit stuffing */
        j = 0;
        count = 0;

        for (i = 0; i < 16; i++)
        {
            stuffed[j++] = binary[i];

            if (binary[i] == '1')
                count++;
            else
                count = 0;

            if (count == 5)
            {
                stuffed[j++] = '0';
                count = 0;
            }
        }

        stuffed[j] = '\0';

        printf("Bit-stuffed binary: %s\n", stuffed);

        /* Send bit string back to client */
        sendto(sockfd, stuffed, strlen(stuffed), 0,
               (struct sockaddr *)&clientAddr,
               addrLen);

        printf("Bit-stuffed string sent\n");
    }
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int sockfd, clientfd, client, choice, n, pending1 = 0, pending2 = 0;
    char message[1024], message1[1024] = "", message2[1024] = "";
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
    {
        printf("Bind failed\n");
        close(sockfd);
        return 1;
    }
    if (listen(sockfd, 5) < 0)
    {
        printf("Listen failed\n");
        close(sockfd);
        return 1;
    }
    printf("=================================\n");
    printf("      TCP CHAT SERVER\n");
    printf("=================================\n");
    printf("Server is running...\n");
    while (1)
    {
        printf("\nWaiting for a client...\n");
        clientfd = accept(sockfd,(struct sockaddr *)&clientAddr,&addrLen);
        if (clientfd < 0)
        {
            printf("Accept failed\n");
            continue;
        }
        n = recv(clientfd,&client,sizeof(client),0);
        if (n <= 0)
        {
            close(clientfd);
            continue;
        }
        printf("Client %d connected\n", client);
        int hasMessage = 0;
        if (client == 1 && pending1 == 1)
        {
            hasMessage = 1;
            send(clientfd,message1,strlen(message1),0);
            printf("Message delivered to Client 1\n");
            pending1 = 0;
        }
        else if (client == 2 && pending2 == 1)
        {
            hasMessage = 1;
            send(clientfd,message2,strlen(message2),0);
            printf("Message delivered to Client 2\n");
            pending2 = 0;
        }
        send(clientfd,&hasMessage,sizeof(hasMessage),0);
        n = recv(clientfd,&choice,sizeof(choice),0);
        if (n <= 0)
        {
            close(clientfd);
            continue;
        }
        if (choice == 2)
        {
            printf("Client %d left the chat\n", client);
            close(clientfd);
            continue;
        }
        if (choice == 1)
        {
            n = recv(clientfd,message,sizeof(message) - 1,0);
            if (n > 0)
            {
                message[n] = '\0';
                printf("Client %d: %s\n",client,message);
                if (client == 1)
                {
                    strcpy(message2, message);
                    pending2 = 1;
                    printf("Message stored for Client 2\n");
                }
                else if (client == 2)
                {
                    strcpy(message1, message);
                    pending1 = 1;
                    printf("Message stored for Client 1\n");
                }
            }
        }
        close(clientfd);
    }
    close(sockfd);
    return 0;
}
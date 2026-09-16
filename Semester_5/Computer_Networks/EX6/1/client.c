#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int sockfd, client, choice, n, hasMessage;
    char message[1024], buffer[1024];
    struct sockaddr_in serverAddr;
    printf("Enter Client Number (1 or 2): ");
    scanf("%d", &client);
    while (1)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            printf("Socket creation failed\n");
            return 1;
        }
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5000);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        if (connect(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
        {
            printf("Connection failed\n");
            close(sockfd);
            return 1;
        }
        send(sockfd,&client,sizeof(client),0);
        n = recv(sockfd, &hasMessage, sizeof(hasMessage),0);
        if (n <= 0)
        {
            printf("Server disconnected\n");
            close(sockfd);
            return 1;
        }
        if (hasMessage == 1)
        {
            n = recv(sockfd,buffer, sizeof(buffer) - 1,0);
            if (n > 0)
            {
                buffer[n] = '\0';
                printf("\n-----------------------------\n");
                printf("Message from other client:\n");
                printf("%s\n", buffer);
                printf("-----------------------------\n");
            }
        }
        printf("\n");
        printf("===== CLIENT %d =====\n", client);
        printf("1. Send Message\n");
        printf("2. Quit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();
        send(sockfd,&choice,sizeof(choice),0);
        if (choice == 1)
        {
            printf("Enter message: ");
            fgets(message,sizeof(message),stdin);
            message[strcspn(message, "\n")] = '\0';
            send(sockfd, message,strlen(message),0);
            printf("Message sent.\n");
        }
        if (choice == 2)
        {
            printf("You left the chat.\n");
            close(sockfd);
            break;
        }
        close(sockfd);
    }
    return 0;
}
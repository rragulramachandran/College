#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_SIZE 1024

int main()
{
    int sockfd;
    int client;
    int choice;
    int n;
    int messageCount;

    char message[MAX_SIZE];
    char buffer[MAX_SIZE];

    struct sockaddr_in serverAddr;

    /* Get client number */
    printf("Enter Client Number (1 or 2): ");
    scanf("%d", &client);

    /* Create socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }

    /* Server address */
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Connect */
    if (connect(sockfd,
                (struct sockaddr *)&serverAddr,
                sizeof(serverAddr)) < 0)
    {
        printf("Connection failed\n");
        close(sockfd);
        return 1;
    }

    /* Send client number */
    send(sockfd,
         &client,
         sizeof(client),
         0);

    /*
     * Receive number of waiting messages
     */
    n = recv(sockfd,
             &messageCount,
             sizeof(messageCount),
             0);

    if (n <= 0)
    {
        printf("Server disconnected\n");
        close(sockfd);
        return 1;
    }

    /*
     * Receive and display all queued messages
     */
    for (int i = 0; i < messageCount; i++)
    {
        n = recv(sockfd,
                 buffer,
                 MAX_SIZE,
                 0);

        if (n > 0)
        {
            buffer[MAX_SIZE - 1] = '\0';

            printf("\n-----------------------------\n");
            printf("Message from other client:\n");
            printf("%s\n", buffer);
            printf("-----------------------------\n");
        }
    }

    /*
     * Display menu
     */
    printf("\n===== CLIENT %d =====\n", client);
    printf("1. Send Message\n");
    printf("2. Quit\n");
    printf("Enter choice: ");

    scanf("%d", &choice);
    getchar();

    /* Send choice */
    send(sockfd,
         &choice,
         sizeof(choice),
         0);

    /*
     * Send message
     */
    if (choice == 1)
    {
        printf("Enter message: ");

        fgets(message,
              sizeof(message),
              stdin);

        message[strcspn(message, "\n")] = '\0';

        send(sockfd,
             message,
             strlen(message),
             0);

        printf("Message sent.\n");
    }

    /*
     * Quit
     */
    if (choice == 2)
    {
        printf("You left the chat.\n");
    }

    /*
     * One turn is finished.
     */
    close(sockfd);

    return 0;
}
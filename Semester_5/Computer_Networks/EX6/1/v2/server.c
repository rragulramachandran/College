#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_MESSAGES 10
#define MAX_SIZE 1024

int main()
{
    int sockfd, clientfd;
    int client;
    int choice;
    int n;
    int messageCount;

    char message[MAX_SIZE];

    /* Queue for Client 1 */
    char queue1[MAX_MESSAGES][MAX_SIZE];
    int count1 = 0;

    /* Queue for Client 2 */
    char queue2[MAX_MESSAGES][MAX_SIZE];
    int count2 = 0;

    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    socklen_t addrLen = sizeof(clientAddr);

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
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    /* Bind */
    if (bind(sockfd,
             (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        printf("Bind failed\n");
        close(sockfd);
        return 1;
    }

    /* Listen */
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

        /* Accept client */
        clientfd = accept(sockfd,
                          (struct sockaddr *)&clientAddr,
                          &addrLen);

        if (clientfd < 0)
        {
            printf("Accept failed\n");
            continue;
        }

        /* Receive client number */
        n = recv(clientfd,
                 &client,
                 sizeof(client),
                 0);

        if (n <= 0)
        {
            close(clientfd);
            continue;
        }

        printf("Client %d connected\n", client);

        /*
         * Find number of messages waiting
         */
        if (client == 1)
            messageCount = count1;
        else
            messageCount = count2;

        /* Send number of waiting messages */
        send(clientfd,
             &messageCount,
             sizeof(messageCount),
             0);

        /*
         * Send all messages in the queue
         */
        if (client == 1)
        {
            for (int i = 0; i < count1; i++)
            {
                /*
                 * Send the complete 1024-byte buffer
                 */
                send(clientfd,
                     queue1[i],
                     MAX_SIZE,
                     0);
            }

            count1 = 0;
        }
        else
        {
            for (int i = 0; i < count2; i++)
            {
                /*
                 * Send the complete 1024-byte buffer
                 */
                send(clientfd,
                     queue2[i],
                     MAX_SIZE,
                     0);
            }

            count2 = 0;
        }

        /*
         * Receive client's choice
         */
        n = recv(clientfd,
                 &choice,
                 sizeof(choice),
                 0);

        if (n <= 0)
        {
            close(clientfd);
            continue;
        }

        /* Quit */
        if (choice == 2)
        {
            printf("Client %d left the chat\n", client);

            close(clientfd);
            continue;
        }

        /* Send message */
        if (choice == 1)
        {
            n = recv(clientfd,
                     message,
                     MAX_SIZE,
                     0);

            if (n > 0)
            {
                message[n] = '\0';

                printf("Client %d: %s\n",
                       client,
                       message);

                /*
                 * Put message into
                 * the other client's queue
                 */

                if (client == 1)
                {
                    if (count2 < MAX_MESSAGES)
                    {
                        strcpy(queue2[count2], message);

                        count2++;

                        printf("Message added to Client 2 queue\n");
                    }
                    else
                    {
                        printf("Client 2 queue is full\n");
                    }
                }
                else
                {
                    if (count1 < MAX_MESSAGES)
                    {
                        strcpy(queue1[count1], message);

                        count1++;

                        printf("Message added to Client 1 queue\n");
                    }
                    else
                    {
                        printf("Client 1 queue is full\n");
                    }
                }
            }
        }

        /* Finish this client's turn */
        close(clientfd);
    }

    close(sockfd);

    return 0;
}
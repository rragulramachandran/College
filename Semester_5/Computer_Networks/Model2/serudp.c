
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int s, n, i;
    char roll[20], result[100];
    struct sockaddr_in addr, client;
    socklen_t len = sizeof(client);

    char *rno[] = {"101", "102", "103"};
    char *name[] = {"Arun", "Bala", "Kumar"};
    char *dept[] = {"CSE", "ECE", "IT"};

    s = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    printf("UDP Student Server waiting...\n");

    n = recvfrom(s, roll, sizeof(roll) - 1, 0,
                 (struct sockaddr *)&client, &len);

    if (n < 0)
    {
        perror("Receive failed");
        close(s);
        return 1;
    }

    roll[n] = '\0';

    for (i = 0; i < 3; i++)
    {
        if (strcmp(roll, rno[i]) == 0)
            break;
    }

    if (i < 3)
        sprintf(result, "Name: %s, Department: %s",
                name[i], dept[i]);
    else
        strcpy(result, "Student not found");

    sendto(s, result, strlen(result), 0,
           (struct sockaddr *)&client, len);

    printf("Request: %s\n", roll);
    printf("Response: %s\n", result);

    close(s);
    return 0;
}

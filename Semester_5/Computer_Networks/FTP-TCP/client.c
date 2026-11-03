
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

int main()
{
    int s;
    struct sockaddr_in server;
    char buffer[1024];
    FILE *fp;

    fp = fopen("input.txt", "rb");

    if (fp == NULL)
    {
        perror("File");
        return 1;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connection");
        fclose(fp);
        close(s);
        return 1;
    }

    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        size_t sent = 0;

        while (sent < n)
        {
            ssize_t k = send(s, buffer + sent, n - sent, 0);

            if (k <= 0)
            {
                perror("Send");
                fclose(fp);
                close(s);
                return 1;
            }

            sent += k;
        }
    }

    printf("File Sent Successfully\n");

    fclose(fp);
    close(s);
    return 0;
}

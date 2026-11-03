
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

int main()
{
    int s, c;
    struct sockaddr_in server, client;
    char buffer[1024];
    socklen_t clientLen = sizeof(client);
    FILE *fp;

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);

    while (1)
    {
        clientLen = sizeof(client);
        c = accept(s, (struct sockaddr *)&client, &clientLen);

        if (c < 0)
            continue;

        fp = fopen("received.txt", "wb");

        if (fp == NULL)
        {
            perror("File");
            close(c);
            continue;
        }

        int n;

        while ((n = recv(c, buffer, sizeof(buffer), 0)) > 0)
        {
            fwrite(buffer, 1, n, fp);
        }

        printf("File Received Successfully\n");

        fclose(fp);
        close(c);
    }

    close(s);
    return 0;
}

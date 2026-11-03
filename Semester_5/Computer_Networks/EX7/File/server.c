#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int connfd;
    char buf[1024];
    struct sockaddr_in saddr, caddr;
    socklen_t len;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("socket");
        return 1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("bind");
        return 1;
    }

    listen(sock, 5);
    printf("File transfer server running on port %s...\n", argv[1]);

    while (1)
    {
        len = sizeof(caddr);
        connfd = accept(sock, (struct sockaddr *)&caddr, &len);

        if (connfd < 0)
        {
            perror("accept");
            continue;
        }

        if (fork() == 0)
        {
            close(sock);
            char filename[256];
            int i = 0;
            char c;

            while (recv(connfd, &c, 1, 0) > 0 && c != '\n' && i < 255)
            {
                filename[i++] = c;
            }
            filename[i] = '\0';
            char spath[300];
            snprintf(spath, sizeof(spath), "received_%s", filename);
            FILE *fp = fopen(spath, "wb");
            if (fp == NULL)
            {
                perror("fopen");
                close(connfd);
                exit(1);
            }

            int n;
            long total = 0;

            while ((n = recv(connfd, buf, sizeof(buf), 0)) > 0)
            {
                fwrite(buf, 1, n, fp);
                total += n;
            }

            fclose(fp);
            printf("Received file '%s' (%ld bytes) -> saved as '%s'\n", filename, total, spath);
            close(connfd);
            exit(0);
        }

        close(connfd);
        waitpid(-1, NULL, WNOHANG);
    }
}

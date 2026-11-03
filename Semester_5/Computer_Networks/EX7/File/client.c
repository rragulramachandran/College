#include <arpa/inet.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char buf[1024];
    char fpath[256];
    struct sockaddr_in saddr;

    if (argc != 3)
    {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    printf("Enter path of file to send: ");
    fgets(fpath, sizeof(fpath), stdin);
    fpath[strcspn(fpath, "\n")] = '\0';
    FILE *fp = fopen(fpath, "rb");
    if (fp == NULL)
    {
        perror("fopen");
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
    saddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &saddr.sin_addr);

    if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("connect");
        return 1;
    }

    char *filename = basename(fpath);
    char header[300];
    snprintf(header, sizeof(header), "%s\n", filename);
    send(sock, header, strlen(header), 0);
    int n;
    long total = 0;

    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0)
    {
        send(sock, buf, n, 0);
        total += n;
    }

    fclose(fp);
    printf("Sent file '%s' (%ld bytes) to server\n", filename, total);
    close(sock);
    return 0;
}

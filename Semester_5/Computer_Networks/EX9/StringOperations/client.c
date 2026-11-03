#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    int s;
    char request[1024], response[1056];
    struct sockaddr_in server = {0};
    socklen_t len = sizeof(server);

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) { perror("socket"); return 1; }

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Commands: reverse <string>, delete <string> <substring>, concat <string> <string>\n");
    printf("Enter command (strings without spaces): ");
    fflush(stdout);
    if (fgets(request, sizeof(request), stdin) == NULL)
    {
        close(s);
        return 1;
    }
    if (strchr(request, '\n') == NULL && !feof(stdin))
    {
        printf("Error: Command too long.\n");
        close(s);
        return 1;
    }

    /* Send the original text so the server validates function names and arguments. */
    if (sendto(s, request, strlen(request), 0,
               (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("sendto");
        close(s);
        return 1;
    }

    ssize_t received = recvfrom(s, response, sizeof(response) - 1, 0,
                                (struct sockaddr *)&server, &len);
    if (received < 0)
    {
        perror("recvfrom");
        close(s);
        return 1;
    }
    response[received] = '\0';
    printf("%s\n", response);

    close(s);
    return 0;
}

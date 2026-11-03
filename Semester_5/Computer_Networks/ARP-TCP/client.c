
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

struct ARP
{
    char ip[20];
    char mac[20];
};

int main()
{
    int s;
    struct sockaddr_in server;
    struct ARP data = {0};

    printf("Enter IP Address : ");
    scanf("%19s", data.ip);

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    int valid = connect(s, (struct sockaddr *)&server, sizeof(server));

    if (valid < 0)
    {
        close(s);
        return 1;
    }

    send(s, &data, sizeof(data), 0);

    int n = recv(s, &data, sizeof(data), MSG_WAITALL);

    if (n == sizeof(data))
        printf("MAC Address : %s\n", data.mac);

    close(s);
    return 0;
}


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
    int s, c;
    struct sockaddr_in server, client;
    socklen_t clientLen = sizeof(client);

    struct ARP data;
    struct ARP records[3] = {
        {"192.168.1.1", "02:00:00:00:00:01"},
        {"192.168.1.2", "02:00:00:00:00:02"},
        {"192.168.1.3", "02:00:00:00:00:03"}
    };

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

        int n = recv(c, &data, sizeof(data), MSG_WAITALL);

        if (n == sizeof(data))
        {
            printf("IP Address : %s\n", data.ip);

            strcpy(data.mac, "Not Found");

            for (int i = 0; i < 3; i++)
            {
                if (strcmp(data.ip, records[i].ip) == 0)
                {
                    strcpy(data.mac, records[i].mac);
                    break;
                }
            }

            send(c, &data, sizeof(data), 0);
        }

        close(c);
    }

    close(s);
    return 0;
}

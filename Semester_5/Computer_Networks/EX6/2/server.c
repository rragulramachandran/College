#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define SIZE 20
#define PORT 5600

struct ARP
{
    char ip[20];
    char mac[20];
};
struct ARP table[SIZE];

int hash(char ip[])
{
    int sum = 0, i;
    for (i = 0; ip[i] != '\0'; i++) sum += ip[i];
    return sum % SIZE;
}

int main()
{
    FILE *fp;
    char line[200], ip[20], mac[20];
    int idx;
    
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int ch;

    srand(time(NULL));
    fp = popen("ip neigh", "r");
    if (fp == NULL)
    {
        printf("Could not read ARP table.\n");
        return 1;
    }
    while (fgets(line, sizeof(line), fp))
    {
        sscanf(line, "%s", ip);
        if (strstr(line, "lladdr") != NULL)
        {
            sscanf(strstr(line, "lladdr") + 7, "%s", mac);
            idx = hash(ip);
            strcpy(table[idx].ip, ip);
            strcpy(table[idx].mac, mac);
        }
    }
    pclose(fp);

    server_fd = socket(AF_INET, IPPROTO_TCP, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);
    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        printf("\nClient connected.\n");

        while (1)
        {
            if (recv(client_fd, &ch, sizeof(ch), 0) <= 0) break;

            if (ch == 3)
            {
                break;
            }
            else if (ch == 2)
            {
                send(client_fd, table, sizeof(table), 0);
            }
            else if (ch == 1)
            {
                recv(client_fd, ip, sizeof(ip), 0);
                idx = hash(ip);
                
                if (strcmp(table[idx].ip, ip) == 0)
                {
                    int found = 1;
                    send(client_fd, &found, sizeof(found), 0);
                    send(client_fd, table[idx].mac, sizeof(table[idx].mac), 0);
                }
                else
                {
                    int found = 0;
                    send(client_fd, &found, sizeof(found), 0);
                    
                    sprintf(mac, "AA:BB:CC:DD:EE:%02X", rand() % 256);
                    strcpy(table[idx].ip, ip);
                    strcpy(table[idx].mac, mac);
                    
                    send(client_fd, table[idx].ip, sizeof(table[idx].ip), 0);
                    send(client_fd, table[idx].mac, sizeof(table[idx].mac), 0);
                }
            }
        }
        close(client_fd);
        printf("Client disconnected. Waiting for next connection...\n");
    }
    close(server_fd);
    return 0;
}


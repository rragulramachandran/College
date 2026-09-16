#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE 20
#define PORT 5600

struct ARP
{
    char ip[20];
    char mac[20];
};
struct ARP table[SIZE];

void display_table()
{
    int i;
    printf("\nARP TABLE READ FROM SERVER\n");
    printf("-------------------------------\n");
    printf("IP Address\tMAC Address\n");
    printf("-------------------------------\n");
    for (i = 0; i < SIZE; i++) if (strlen(table[i].ip) > 0) printf("%s\t%s\n", table[i].ip, table[i].mac);
    printf("-------------------------------\n");
}

int main()
{
    char ip[20], mac[20];
    int sock_fd;
    struct sockaddr_in server_addr;
    int ch;

    sock_fd = socket(AF_INET, IPPROTO_TCP, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Connection to server failed.\n");
        return 1;
    }

    while (1)
    {
        printf("Enter choice (1. Search, 2. Display, 3. Exit): ");
        scanf("%d", &ch);
        
        send(sock_fd, &ch, sizeof(ch), 0);

        if (ch == 3) break;
        else if (ch == 2)
        {
            recv(sock_fd, table, sizeof(table), 0);
            display_table();
        }
        else if (ch == 1)
        {
            printf("\nEnter IP address: ");
            scanf("%s", ip);
            send(sock_fd, ip, sizeof(ip), 0);

            int found;
            recv(sock_fd, &found, sizeof(found), 0);

            if (found == 1)
            {
                recv(sock_fd, mac, sizeof(mac), 0);
                printf("MAC Address found: %s\n", mac);
            }
            else
            {
                char new_ip[20], new_mac[20];
                printf("MAC Address not found.\n");
                recv(sock_fd, new_ip, sizeof(new_ip), 0);
                recv(sock_fd, new_mac, sizeof(new_mac), 0);
                printf("Adding entry to program memory...\n");
                printf("IP  : %s\n", new_ip);
                printf("MAC : %s\n", new_mac);
            }
        }
        else printf("Invalid choice.\n");
    }

    close(sock_fd);
    return 0;
}


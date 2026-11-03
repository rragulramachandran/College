#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct ARP
{
    char ip[20];
    char mac[20];
    int used;
};
struct ARP arr[10];
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

int hash(char ip[])
{
    int value = 0;
    for (int i = 0; ip[i] != '\0'; i++)
    {
        value += ip[i];
    }
    return value % 10;
}

void insert(char ip[], char mac[])
{
    int index = hash(ip), start = index;

    while (arr[index].used == 1)
    {
        index = (index + 1) % 10;
        if (index == start)
        {
            printf("ARP Table is full! Cannot insert %s\n", ip);
            return;
        }
    }

    strcpy(arr[index].ip, ip);
    strcpy(arr[index].mac, mac);
    arr[index].used = 1;
}

int search(char ip[], char mac[])
{
    int index = hash(ip), start = index;

    while (arr[index].used == 1)
    {
        if (strcmp(arr[index].ip, ip) == 0)
        {
            strcpy(mac, arr[index].mac);
            return 1;
        }

        index = (index + 1) % 10;

        if (index == start)
        {
            break;
        }
    }
    return 0;
}

void gen_mac(char ip[], char mac[])
{
    int a, b, c, d;
    if (sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d) == 4)
    {
        sprintf(mac, "AA:BB:%02X:%02X:%02X:%02X", a, b, c, d);
    }
    else
    {
        strcpy(mac, "00:00:00:00:00:00");
    }
}

void show_arp(void)
{
    printf("\n============ CURRENT SIMULATED ARP TABLE ============\n");
    printf("%-5s %-18s %-20s\n", "Slot", "IP Address", "MAC Address");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < 10; i++)
    {
        printf("[%02d]  %-18s %-20s\n", i, arr[i].used == 1 ? arr[i].ip : "[Empty]",
               arr[i].used == 1 ? arr[i].mac : "None");
    }
    printf("=====================================================\n\n");
}

void show_sys_arp(void)
{
    printf("\n============ SYSTEM ARP CACHE (arp -a) ============\n");
    FILE *fp = popen("arp -a", "r");
    if (fp == NULL)
    {
        perror("popen (arp -a) failed");
        return;
    }

    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
        found = 1;
    }

    if (!found)
    {
        printf("(No entries returned by 'ip neigh')\n");
    }

    pclose(fp);
    printf("=====================================================\n\n");
}

void *client_handler(void *arg)
{
    int connfd = *(int *)arg;
    free(arg);
    char buf[1024], mac[20], reply[1024];
    int n = recv(connfd, buf, sizeof(buf) - 1, 0);

    if (n > 0)
    {
        buf[n] = '\0';
        buf[strcspn(buf, "\r\n")] = '\0';
        struct in_addr test_addr;
        if (inet_pton(AF_INET, buf, &test_addr) != 1)
        {
            char *msg = "Invalid IP address\n";
            send(connfd, msg, strlen(msg), 0);
        }
        else
        {
            pthread_mutex_lock(&mlock);
            const char *status = "IP found in ARP arr.\n";
            if (!search(buf, mac))
            {
                gen_mac(buf, mac);
                insert(buf, mac);
                status = "IP not found. Added to ARP arr.\n";
            }
            printf("%s", status);
            sprintf(reply, "IP: %s\nMAC: %s\n%s", buf, mac, status);
            show_arp();
            show_sys_arp();
            pthread_mutex_unlock(&mlock);
            send(connfd, reply, strlen(reply), 0);
        }
    }

    close(connfd);
    return NULL;
}

int main(int argc, char *argv[])
{
    int connfd;
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

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
    {
        perror("bind");
        close(sock);
        return 1;
    }

    if (listen(sock, 5) < 0)
    {
        perror("listen");
        close(sock);
        return 1;
    }

    insert("192.168.1.1", "AA:BB:CC:DD:EE:01");
    insert("192.168.1.2", "AA:BB:CC:DD:EE:02");
    insert("192.168.1.3", "AA:BB:CC:DD:EE:03");
    insert("192.168.1.4", "AA:BB:CC:DD:EE:04");
    printf("ARP lookup server running on port %s...\n", argv[1]);
    show_arp();
    show_sys_arp();

    while (1)
    {
        len = sizeof(caddr);
        connfd = accept(sock, (struct sockaddr *)&caddr, &len);
        if (connfd < 0)
        {
            perror("accept");
            continue;
        }

        printf("Client connected.\n");
        int *client_sock_ptr = malloc(sizeof(int));
        if (client_sock_ptr == NULL)
        {
            perror("malloc failed");
            close(connfd);
            continue;
        }
        *client_sock_ptr = connfd;
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handler, client_sock_ptr) != 0)
        {
            perror("pthread_create failed");
            free(client_sock_ptr);
            close(connfd);
            continue;
        }

        pthread_detach(thread_id);
    }
}

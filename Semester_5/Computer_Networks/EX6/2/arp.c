#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <time.h>
#define SIZE 20
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
void display_table()
{
    int i;
    printf("\nARP TABLE READ FROM PC\n");
    printf("-------------------------------\n");
    printf("IP Address\tMAC Address\n");
    printf("-------------------------------\n");
    for (i = 0; i < SIZE; i++) if (strlen(table[i].ip) > 0) printf("%s\t%s\n", table[i].ip, table[i].mac);
    printf("-------------------------------\n");
}
int main()
{
    FILE *fp;
    char line[200], ip[20], mac[20];
    int idx;
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
    display_table();
    while (1)
    {
        int ch;
        printf("Enter choice (1. Search, 2. Display, 3. Exit): ");
        scanf("%d", &ch);
        if (ch == 3) break;
        else if (ch == 2) display_table();
        else if (ch == 1)
        {
            printf("\nEnter IP address: ");
            scanf("%s", ip);
            idx = hash(ip);
            if (strcmp(table[idx].ip, ip) == 0) printf("MAC Address found: %s\n", table[idx].mac);
            else
            {
                printf("MAC Address not found.\n");
                sprintf(mac,"AA:BB:CC:DD:EE:%02X",rand() % 256);
                strcpy(table[idx].ip, ip);
                strcpy(table[idx].mac, mac);
                printf("Adding entry to program memory...\n");
                printf("IP  : %s\n", table[idx].ip);
                printf("MAC : %s\n", table[idx].mac);
            }
        }
        else printf("Invalid choice.\n");
    }
    return 0;
}
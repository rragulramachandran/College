#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char ip_query[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    // 1. Create Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n Invalid address \n");
        return -1;
    }

    // 2. Connect to ARP Server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Connection Failed! Ensure server is running.\n");
        return -1;
    }

    printf("[+] Connected to ARP Server.\n");
    printf("[+] Type 'exit' or 'bye' anytime to disconnect.\n");

    // CONTINUOUS QUERY LOOP
    while (1) {
        printf("\nEnter Target IP Address (e.g., 192.168.1.1): ");
        memset(ip_query, 0, BUFFER_SIZE);
        
        if (fgets(ip_query, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        ip_query[strcspn(ip_query, "\r\n")] = 0; // Remove trailing newline

        // Check for exit condition
        if (strncmp(ip_query, "exit", 4) == 0 || strncmp(ip_query, "bye", 3) == 0) {
            send(sock, "exit", 4, 0);
            printf("[-] Disconnecting from ARP server. Goodbye!\n");
            break;
        }

        if (strlen(ip_query) == 0) continue;

        // Send IP query to server
        send(sock, ip_query, strlen(ip_query), 0);

        // Receive MAC resolution response
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received > 0) {
            printf("[RESULT] ARP Resolution for %s:\n", ip_query);
            printf("  -> MAC Address: %s\n", buffer);
        } else {
            printf("[-] Server closed connection or no response.\n");
            break;
        }
    }

    close(sock);
    return 0;
}
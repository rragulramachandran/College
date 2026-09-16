#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define HASH_SIZE 10

// Node structure for Hash Table linked list (Chaining)
struct Node {
    char ip[32];
    char mac[32];
    struct Node* next;
};

// Hash Table array
struct Node* hashTable[HASH_SIZE];

// Simple Hash Function for IP string
unsigned int hash_function(const char* ip) {
    unsigned int hash = 0;
    while (*ip) {
        hash = (hash * 31) + *ip++;
    }
    return hash % HASH_SIZE;
}

// Insert IP -> MAC mapping into Hash Table
void insert_arp(const char* ip, const char* mac) {
    unsigned int index = hash_function(ip);
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->ip, ip);
    strcpy(newNode->mac, mac);
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

// Lookup MAC address by IP in Hash Table
char* lookup_arp(const char* ip) {
    unsigned int index = hash_function(ip);
    struct Node* current = hashTable[index];
    
    while (current != NULL) {
        if (strcmp(current->ip, ip) == 0) {
            return current->mac;
        }
        current = current->next;
    }
    return NULL; // Not found
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Initialize Hash Table with NULLs
    for (int i = 0; i < HASH_SIZE; i++) hashTable[i] = NULL;

    // Populate static ARP Table (IP -> MAC mappings)
    insert_arp("192.168.1.1", "AA:BB:CC:DD:EE:01");
    insert_arp("192.168.1.2", "AA:BB:CC:DD:EE:02");
    insert_arp("192.168.1.3", "AA:BB:CC:DD:EE:03");
    insert_arp("10.0.0.1",    "11:22:33:44:55:66");

    // 1. Create Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind Socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[+] ARP Server (Hash Table Lookup) started on port %d...\n", PORT);

    // OUTER ITERATIVE LOOP: Waits for one client after another sequentially
    while (1) {
        printf("\n---------------------------------------------\n");
        printf("[+] Waiting for a client connection...\n");

        if ((client_socket = accept(server_fd, (struct sockaddr )&address, (socklen_t)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        printf("[+] Client connected successfully!\n");

        // INNER SESSION LOOP: Handles multiple IP queries from the active client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            
            if (bytes_read <= 0 || strncmp(buffer, "exit", 4) == 0 || strncmp(buffer, "bye", 3) == 0) {
                printf("[-] Client ended ARP session.\n");
                break;
            }

            buffer[strcspn(buffer, "\r\n")] = 0; // Strip trailing newline
            printf("[+] Received ARP Request for IP: %s\n", buffer);

            char* mac = lookup_arp(buffer);
            if (mac != NULL) {
                printf("[+] Resolved MAC: %s\n", mac);
                send(client_socket, mac, strlen(mac), 0);
            } else {
                printf("[-] IP Not Found in ARP Cache Table.\n");
                send(client_socket, "ERROR: MAC Address Not Found", 28, 0);
            }
        }

        close(client_socket);
        printf("[+] Connection closed. Server ready for next client in queue.\n");
    }

    close(server_fd);
    return 0;
}
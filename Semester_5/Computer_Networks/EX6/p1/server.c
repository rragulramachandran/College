#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept connection
    new_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (new_sock < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected! Start chatting (type 'exit' to quit).\n\n");

    // Chat loop
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // 1. Receive message from Client
        ssize_t bytes_received = recv(new_sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0 || strncmp(buffer, "exit", 4) == 0) {
            printf("Client disconnected.\n");
            break;
        }
        printf("Client: %s", buffer);

        // 2. Read response from Server terminal
        printf("Server: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // 3. Send message to Client
        send(new_sock, buffer, strlen(buffer), 0);
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Ending chat...\n");
            break;
        }
    }

    close(new_sock);
    close(server_fd);
    return 0;
}
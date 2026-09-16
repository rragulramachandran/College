#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5555
#define BUFFER_SIZE 1024

void handle_retrieve(int client_socket, const char *filename) {
    char buffer[BUFFER_SIZE];
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) {
        printf("File '%s' not found on server.\n", filename);
        send(client_socket, "ERROR: File not found", 21, 0);
        return;
    }

    send(client_socket, "OK", 2, 0);
    usleep(10000);

    printf(" Sending file '%s' to client...\n", filename);
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
        memset(buffer, 0, BUFFER_SIZE);
    }
    fclose(fp);
    printf(" File '%s' sent successfully!\n", filename);
}

void handle_store(int client_socket, const char *filename) {
    char buffer[BUFFER_SIZE];
    char save_name[BUFFER_SIZE + 30];

    snprintf(save_name, sizeof(save_name), "uploaded_%s", filename);
    FILE *fp = fopen(save_name, "wb");

    if (fp == NULL) {
        perror(" Failed to create file on server");
        send(client_socket, "ERROR: Server storage failure", 29, 0);
        return;
    }

    send(client_socket, "OK", 2, 0);
    printf(" Receiving file from client, saving as '%s'...\n", save_name);

    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        if (strncmp(buffer, "EOF_SIGNAL", 10) == 0) break;
        fwrite(buffer, 1, bytes_received, fp);
        memset(buffer, 0, BUFFER_SIZE);
    }

    fclose(fp);
    printf("File saved successfully as '%s'.\n", save_name);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char command[BUFFER_SIZE];
    char filename[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf(" Iterative TCP FTP Server running on port %d...\n", PORT);

    while (1) {
        printf("\n---------------------------------------------\n");
        printf(" Server ready. Waiting for connection...\n");

        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        printf(" Client connected!\n");

        while (1) {
            memset(command, 0, BUFFER_SIZE);
            memset(filename, 0, BUFFER_SIZE);

            int bytes_read = recv(client_socket, command, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0 || strncmp(command, "EXIT", 4) == 0) {
                printf(" Client requested disconnect or connection closed.\n");
                break;
            }

            command[strcspn(command, "\r\n")] = 0;

            if (strncmp(command, "RETRIEVE", 8) == 0) {
                recv(client_socket, filename, BUFFER_SIZE - 1, 0);
                filename[strcspn(filename, "\r\n")] = 0;
                printf(" Client requested RETRIEVE for file: '%s'\n", filename);
                handle_retrieve(client_socket, filename);

            } else if (strncmp(command, "STORE", 5) == 0) {
                recv(client_socket, filename, BUFFER_SIZE - 1, 0);
                filename[strcspn(filename, "\r\n")] = 0;
                printf(" Client requested STORE for file: '%s'\n", filename);
                handle_store(client_socket, filename);
            }
        }

        close(client_socket);
        printf("Connection closed. Waiting for next client in queue...\n");
    }

    close(server_fd);
    return 0;
}
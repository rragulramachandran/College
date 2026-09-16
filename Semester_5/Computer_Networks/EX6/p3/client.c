#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void store_file(int sock) {
    char filename[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    printf("Enter local filename to store on server (upload): ");
    fgets(filename, BUFFER_SIZE, stdin);
    filename[strcspn(filename, "\r\n")] = 0;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("[-] Local file '%s' does not exist!\n", filename);
        return;
    }

    // Send STORE command then filename
    send(sock, "STORE", 5, 0);
    usleep(10000);
    send(sock, filename, strlen(filename), 0);

    // Wait for server acknowledgement
    memset(buffer, 0, BUFFER_SIZE);
    recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (strncmp(buffer, "OK", 2) != 0) {
        printf("[-] Server rejected store request: %s\n", buffer);
        fclose(fp);
        return;
    }

    printf("[+] Uploading '%s' to server...\n", filename);
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(sock, buffer, bytes_read, 0);
        memset(buffer, 0, BUFFER_SIZE);
    }
    fclose(fp);

    // Signal end of file transfer to server
    usleep(50000);
    send(sock, "EOF_SIGNAL", 10, 0);
    printf("[+] File '%s' successfully uploaded!\n", filename);
}

void retrieve_file(int sock) {
    char filename[BUFFER_SIZE];
    char save_name[BUFFER_SIZE + 30];
    char buffer[BUFFER_SIZE];

    printf("Enter filename to retrieve from server (download): ");
    fgets(filename, BUFFER_SIZE, stdin);
    filename[strcspn(filename, "\r\n")] = 0;

    // Send RETRIEVE command then filename
    send(sock, "RETRIEVE", 8, 0);
    usleep(10000);
    send(sock, filename, strlen(filename), 0);

    // Wait for server status
    memset(buffer, 0, BUFFER_SIZE);
    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (strncmp(buffer, "OK", 2) != 0) {
        printf("[-] Server response: %s\n", buffer);
        return;
    }

    snprintf(save_name, sizeof(save_name), "downloaded_%s", filename);
    FILE *fp = fopen(save_name, "wb");
    if (fp == NULL) {
        perror("[-] Error creating local file");
        return;
    }

    printf("[+] Downloading '%s' and saving as '%s'...\n", filename, save_name);

    int bytes_received;
    while ((bytes_received = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);
        if (bytes_received < BUFFER_SIZE) break; // End of file chunk reached
        memset(buffer, 0, BUFFER_SIZE);
    }

    fclose(fp);
    printf("[+] File successfully downloaded and saved as '%s'!\n", save_name);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int choice;

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

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Connection Failed! Make sure server is running.\n");
        return -1;
    }

    printf("[+] Connected to FTP Server successfully!\n");

    // Continuous Menu Loop until user chooses Exit
    while (1) {
        printf("\n================ FTP MENU ================\n");
        printf("1. Store File (Upload client -> server)\n");
        printf("2. Retrieve File (Download server -> client)\n");
        printf("3. Exit\n");
        printf("Enter choice (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // clear buffer
            continue;
        }
        getchar(); // consume trailing newline

        if (choice == 1) {
            store_file(sock);
        } else if (choice == 2) {
            retrieve_file(sock);
        } else if (choice == 3) {
            send(sock, "EXIT", 4, 0);
            printf("[-] Disconnecting from server. Goodbye!\n");
            break;
        } else {
            printf("[-] Invalid choice! Please select 1, 2, or 3.\n");
        }
    }

    close(sock);
    return 0;
}
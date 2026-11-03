#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Function to convert text to 8-bit binary string
void textToBinary(char *text, char *binary) {
    binary[0] = '\0';
    for (int i = 0; text[i] != '\0'; i++) {
        char byte[9];
        unsigned char ch = text[i];
        for (int k = 7; k >= 0; k--) {
            byte[7 - k] = ((ch >> k) & 1) ? '1' : '0';
        }
        byte[8] = '\0';
        strcat(binary, byte);
    }
}

// Function to perform bit stuffing
void bitStuffing(char *input, char *stuffed) {
    int i = 0, j = 0, count = 0;
    while (input[i] != '\0') {
        if (input[i] == '1') {
            count++;
            stuffed[j++] = input[i];
        } else {
            stuffed[j++] = input[i];
            count = 0;
        }

        if (count == 5) {
            stuffed[j++] = '0'; // Stuff '0' after five consecutive 1s
            count = 0;
        }
        i++;
    }
    stuffed[j] = '\0';
}

int main()
{
    int sock = 0, choice;
    char message[256], binary[2048], stuffed[4096], buffer[256];
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket creation error\n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5002);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return 1;
    }

    while (1) {
        printf("\n1. Send Message (Text -> Binary -> Bit Stuffing)\n");
        printf("2. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        if (choice == 2) break;

        if (choice == 1) {
            printf("Enter text message: ");
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = '\0';

            // 1. Convert Text to Binary
            textToBinary(message, binary);
            printf("Binary Representation: %s\n", binary);

            // 2. Perform Bit Stuffing
            bitStuffing(binary, stuffed);
            printf("Data after Bit Stuffing: %s\n", stuffed);

            // Send stuffed data over TCP
            write(sock, stuffed, strlen(stuffed));

            // Read response from server
            int valread = read(sock, buffer, sizeof(buffer) - 1);
            if (valread > 0) {
                buffer[valread] = '\0';
                printf("Server Response (Recovered Text): %s\n", buffer);
            }
        } else {
            printf("Invalid choice\n");
        }
    }

    close(sock);
    return 0;
}
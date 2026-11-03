#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Function to perform bit destuffing
void bitDestuffing(char *stuffed, char *destuffed) {
  int i = 0, j = 0, count = 0;
  while (stuffed[i] != '\0') {
    if (stuffed[i] == '1') {
      count++;
      destuffed[j++] = stuffed[i];
    } else {
      destuffed[j++] = stuffed[i];
      count = 0;
    }

    if (count == 5) {
      i++; // skip the stuffed '0'
      count = 0;
    }
    i++;
  }
  destuffed[j] = '\0';
}

// Function to convert binary string back to text characters
void binaryToText(char *binary, char *text) {
  int len = strlen(binary);
  int t = 0;
  for (int i = 0; i < len; i += 8) {
    char byte[9];
    for (int k = 0; k < 8; k++) {
      if (i + k < len)
        byte[k] = binary[i + k];
      else
        byte[k] = '0';
    }
    byte[8] = '\0';

    // Convert 8-bit binary string to character
    char ch = 0;
    for (int k = 0; k < 8; k++) {
      ch = (ch << 1) | (byte[k] - '0');
    }
    text[t++] = ch;
  }
  text[t] = '\0';
}

int main() {
  int server_fd, client_fd;
  char stuffed_buffer[2048], destuffed_buffer[2048], text_buffer[1024];
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    printf("Socket creation failed\n");
    return 1;
  }
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
      perror("setsockopt");
      return 1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(5002);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    printf("Bind failed\n");
    close(server_fd);
    return 1;
  }

  if (listen(server_fd, 3) < 0) {
    printf("Listen failed\n");
    close(server_fd);
    return 1;
  }

  printf("TCP Bit Stuffing Server is running...\n");

  while (1) {
    client_fd =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (client_fd < 0) {
      printf("Accept failed\n");
      continue;
    }

    int valread = read(client_fd, stuffed_buffer, sizeof(stuffed_buffer) - 1);
    if (valread > 0) {
      stuffed_buffer[valread] = '\0';
      printf("\nReceived Stuffed Binary Data: %s\n", stuffed_buffer);

      // 1. Perform Bit Destuffing
      bitDestuffing(stuffed_buffer, destuffed_buffer);
      printf("Destuffed Binary Data: %s\n", destuffed_buffer);

      // 2. Convert Binary back to Text
      binaryToText(destuffed_buffer, text_buffer);
      printf("Recovered Text Message: %s\n", text_buffer);

      // Send back the recovered text as confirmation
      write(client_fd, text_buffer, strlen(text_buffer));
      printf("Echoed recovered text back to client.\n");
    }

    close(client_fd);
  }

  close(server_fd);
  return 0;
}
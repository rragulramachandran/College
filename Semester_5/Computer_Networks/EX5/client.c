#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  char message[1024], buffer[1024];

  // 1. Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    printf("Socket creation error\n");
    return 1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(6000);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // 2. Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("Connection Failed\n");
    return 1;
  }

  // 3. Get message from user
  printf("Enter message: ");
  fgets(message, sizeof(message), stdin);
  message[strcspn(message, "\n")] = '\0';

  // 4. Send message to server
  send(sock, message, strlen(message), 0);

  // 5. Read echo from server
  int valread = read(sock, buffer, 1024);
  buffer[valread] = '\0';
  printf("Server Echo: %s\n", buffer);

  // 6. Close socket
  close(sock);
  return 0;
}
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


int main() {
  int server_fd, client_fd;
  char buffer[1024];
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  // 1. Create socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    printf("Socket creation failed\n");
    return 1;
  }

  // 2. Bind to port 6000
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
      perror("setsockopt");
      return 1;
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(6000);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    printf("Bind failed\n");
    return 1;
  }

  // 3. Listen for connections
  if (listen(server_fd, 3) < 0) {
    printf("Listen failed\n");
    return 1;
  }

  printf("TCP Echo Server is running...\n");

  // 4. Accept client connection
  client_fd =
      accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
  if (client_fd < 0) {
    printf("Accept failed\n");
    return 1;
  }

  // 5. Receive message from client
  int valread = read(client_fd, buffer, 1024);
  buffer[valread] = '\0';
  printf("Client: %s\n", buffer);

  // 6. Echo back to client
  send(client_fd, buffer, strlen(buffer), 0);
  printf("Echo sent to client\n");

  // 7. Close sockets
  close(client_fd);
  close(server_fd);
  return 0;
}
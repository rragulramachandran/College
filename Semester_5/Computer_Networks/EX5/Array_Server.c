#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int server_fd, client_fd, n, i;
  int arr[100];
  int sum;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  // 1. Create socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    printf("Socket creation failed\n");
    return 1;
  }

  // 2. Bind to port 5001
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
      perror("setsockopt");
      return 1;
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(5001);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    printf("Bind failed\n");
    close(server_fd);
    return 1;
  }

  // 3. Listen for connections
  if (listen(server_fd, 3) < 0) {
    printf("Listen failed\n");
    close(server_fd);
    return 1;
  }

  printf("TCP Sum Server is running...\n");

  while (1) {
    // 4. Accept client connection
    client_fd =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (client_fd < 0) {
      printf("Accept failed\n");
      continue;
    }

    // 5. Receive array size (n)
    read(client_fd, &n, sizeof(n));

    // 6. Receive array elements
    read(client_fd, arr, n * sizeof(int));

    printf("Received %d elements from client.\n", n);

    // 7. Calculate sum
    sum = 0;
    for (i = 0; i < n; i++) {
      sum += arr[i];
    }

    // 8. Send sum back to client
    write(client_fd, &sum, sizeof(sum));
    printf("Sum (%d) sent to client\n", sum);

    // 9. Close client connection
    close(client_fd);
  }

  close(server_fd);
  return 0;
}
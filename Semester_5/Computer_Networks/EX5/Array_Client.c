#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sock = 0, choice, n, i;
  int arr[100];
  int sum;
  struct sockaddr_in serv_addr;

  // 1. Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    printf("Socket creation error\n");
    return 1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5001);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // 2. Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("Connection Failed\n");
    return 1;
  }

  while (1) {
    printf("\n1. Send Array for Sum\n");
    printf("2. Exit\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 2)
      break;

    if (choice == 1) {
      printf("Enter number of elements: ");
      scanf("%d", &n);

      printf("Enter %d elements:\n", n);
      for (i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
      }

      // Send array size first, then the array elements over TCP stream
      write(sock, &n, sizeof(n));
      write(sock, arr, n * sizeof(int));

      // Read the sum result from server
      read(sock, &sum, sizeof(sum));
      printf("Sum of array elements from Server: %d\n", sum);
    } else {
      printf("Invalid choice\n");
    }
  }

  close(sock);
  return 0;
}
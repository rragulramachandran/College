#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

struct Frame
{
  int seq, ack, drop;
  char data[1024];
};

int main(void)
{
  int s = socket(AF_INET, SOCK_DGRAM, 0), seq = 0, mode;
  int front = 0, rear = 0, count = 0;
  char queue[10][1024], input[1024];
  struct Frame f = {0}, ack;
  struct sockaddr_in server = {.sin_family = AF_INET,
                               .sin_port = htons(8085),
                               .sin_addr.s_addr = htonl(INADDR_LOOPBACK)};
  struct timeval timeout = {.tv_sec = 3};
  if (s < 0)
  {
    perror("socket");
    return 1;
  }
  if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0)
  {
    perror("timeout");
    close(s);
    return 1;
  }
  puts("1.Normal  2.Drop/retry  3.Duplicate");
  if (!fgets(input, sizeof input, stdin) || sscanf(input, "%d", &mode) != 1 ||
      mode < 1 || mode > 3)
  {
    close(s);
    return 1;
  }
  while (1)
  {
    printf("Message (exit to quit): ");
    if (!fgets(input, sizeof input, stdin))
      break;
    input[strcspn(input, "\n")] = 0;
    if (!strcmp(input, "exit"))
      break;
    if (count == 10)
    {
      puts("Queue full");
      continue;
    }
    strcpy(queue[rear], input);
    rear = (rear + 1) % 10;
    count++;
    while (count)
    {
      strcpy(f.data, queue[front]);
      front = (front + 1) % 10;
      count--;
      f.seq = seq;
      f.drop = mode == 2 ? 2 : 0;
      while (1)
      {
        printf("Sending frame %d\n", seq);
        sendto(s, &f, sizeof f, 0, (void *)&server, sizeof server);
        int n = recvfrom(s, &ack, sizeof ack, 0, NULL, NULL);
        if (n == sizeof ack && ack.ack == (seq ^ 1))
          break;
        puts(n < 0 ? "Timeout: retrying" : "Invalid ACK: retrying");
        f.drop = 0;
      }
      printf("Received ACK %d\n", ack.ack);
      seq ^= 1;
      if (mode == 3)
      {
        printf("Resend duplicate? (y/n): ");
        if (fgets(input, sizeof input, stdin) &&
            (input[0] == 'y' || input[0] == 'Y'))
        {
          sendto(s, &f, sizeof f, 0, (void *)&server, sizeof server);
          if (recvfrom(s, &ack, sizeof ack, 0, NULL, NULL) == sizeof ack)
            printf("Duplicate ACK %d\n", ack.ack);
          else
            puts("No duplicate ACK received");
        }
      }
    }
  }
  close(s);
  return 0;
}

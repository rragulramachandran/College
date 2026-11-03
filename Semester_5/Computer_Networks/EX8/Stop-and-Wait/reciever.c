#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

struct Frame
{
  int seq, ack, drop;
  char data[1024];
};

int main(void)
{
  int s = socket(AF_INET, SOCK_DGRAM, 0), expected = 0;
  struct sockaddr_in server = {.sin_family = AF_INET,
                               .sin_port = htons(8085),
                               .sin_addr.s_addr = INADDR_ANY},
                     client;
  struct Frame f, reply = {0};
  if (s < 0)
  {
    perror("socket");
    return 1;
  }
  if (bind(s, (void *)&server, sizeof server) < 0)
  {
    perror("bind");
    close(s);
    return 1;
  }
  puts("Receiver ready on port 8085");
  while (1)
  {
    socklen_t len = sizeof client;
    if (recvfrom(s, &f, sizeof f, 0, (void *)&client, &len) != sizeof f)
      continue;
    if (f.drop == 2)
    {
      printf("Dropped frame %d\n", f.seq);
      continue;
    }
    if (f.seq == expected)
    {
      printf("Accepted %d: %.1024s\n", f.seq, f.data);
      expected ^= 1;
    }
    else
      printf("Duplicate %d discarded\n", f.seq);
    reply.ack = expected;
    sendto(s, &reply, sizeof reply, 0, (void *)&client, len);
    printf("Sent ACK %d\n", expected);
  }
}

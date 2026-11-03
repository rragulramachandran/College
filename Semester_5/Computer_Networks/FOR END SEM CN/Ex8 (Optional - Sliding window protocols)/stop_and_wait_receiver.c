#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

struct Frame {
    int seq;
    char data;
    int total;
};

int main() {
    int sock, expected = 0, count = 0, ack;
    char message[100] = "";
    struct sockaddr_in me, sender;
    socklen_t len;
    struct Frame f;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    me.sin_family = AF_INET;
    me.sin_port = htons(9000);
    me.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr *)&me, sizeof(me));

    printf("Receiver waiting...\n");

    while (1) {
        len = sizeof(sender);
        recvfrom(sock, &f, sizeof(f), 0, (struct sockaddr *)&sender, &len);

        if (f.seq == expected) {
            printf("Frame seq %d ('%c') accepted\n", f.seq, f.data);
            message[count++] = f.data;
            message[count] = '\0';
            expected = (expected + 1) % 2;
        } else {
            printf("Duplicate frame seq %d discarded\n", f.seq);
        }

        ack = expected;                     /* ACK = next expected seq */
        sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&sender, len);
        printf("ACK %d sent\n", ack);

        if (count == f.total) break;        /* all frames received */
    }

    printf("\nMessage received: %s\n", message);
    close(sock);
    return 0;
}

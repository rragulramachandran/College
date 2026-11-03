#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define WINDOW 4

struct Frame {
    int seq;
    char data;
    int total;
};

int main() {
    int sock, base = 0, ack;
    char buffer[100];           /* holds out-of-order frames */
    int received[100] = {0};
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

        if (f.seq >= base && f.seq < base + WINDOW) {
            /* inside receive window: store it (even if out of order) */
            if (!received[f.seq]) {
                buffer[f.seq] = f.data;
                received[f.seq] = 1;
                printf("Frame %d ('%c') stored in buffer\n", f.seq, f.data);
            }

            /* deliver all frames that are now in order */
            while (received[base]) {
                message[base] = buffer[base];
                message[base + 1] = '\0';
                printf("Frame %d delivered. Message so far: %s\n", base, message);
                base++;
            }
        } else if (f.seq < base) {
            printf("Duplicate frame %d (already delivered)\n", f.seq);
        }

        ack = f.seq;            /* ACK this exact frame */
        sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&sender, len);
        printf("ACK %d sent\n", ack);

        if (base == f.total) break;
    }

    printf("\nMessage received: %s\n", message);
    close(sock);
    return 0;
}

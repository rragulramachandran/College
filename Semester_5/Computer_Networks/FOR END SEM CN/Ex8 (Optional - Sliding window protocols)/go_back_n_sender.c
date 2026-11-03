#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define WINDOW 4

struct Frame {
    int seq;        /* frame number: 0,1,2,... */
    char data;
    int total;
};

int main() {
    int sock, n, lose;
    char word[100];
    struct sockaddr_in receiver;
    struct Frame f;
    int ack;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(9000);
    receiver.sin_addr.s_addr = inet_addr("127.0.0.1");

    struct timeval tv = {2, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    printf("Enter word: ");
    scanf("%s", word);
    n = strlen(word);
    printf("Window size = %d\n", WINDOW);
    printf("Frame number to lose once (0 = none): ");
    scanf("%d", &lose);

    int base = 0;       /* oldest unacknowledged frame */
    int next = 0;       /* next frame to send */
    int lostDone = 0;

    while (base < n) {

        /* 1. Send all frames that fit in the window */
        while (next < n && next < base + WINDOW) {
            f.seq = next;
            f.data = word[next];
            f.total = n;

            if (lose == next + 1 && !lostDone) {
                printf("Frame %d ('%c') LOST (not sent)\n", next, f.data);
                lostDone = 1;
            } else {
                sendto(sock, &f, sizeof(f), 0, (struct sockaddr *)&receiver, sizeof(receiver));
                printf("Sent frame %d ('%c')\n", next, f.data);
            }
            next++;
        }

        /* 2. Wait for a cumulative ACK */
        if (recvfrom(sock, &ack, sizeof(ack), 0, NULL, NULL) > 0) {
            printf("ACK %d received (frames up to %d are OK)\n", ack, ack - 1);
            if (ack > base) base = ack;         /* slide the window */
        } else {
            /* 3. Timeout: go back and resend everything from base */
            printf("TIMEOUT! Go back to frame %d\n", base);
            next = base;
        }
    }

    printf("\nAll frames sent successfully.\n");
    close(sock);
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define WINDOW 4

struct Frame {
    int seq;
    char data;
    int total;
};

int main() {
    int sock, n, lose, i;
    char word[100];
    int acked[100] = {0};
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

    int base = 0, next = 0, lostDone = 0;

    while (base < n) {

        /* 1. Send all new frames that fit in the window */
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

        /* 2. Wait for an individual ACK */
        if (recvfrom(sock, &ack, sizeof(ack), 0, NULL, NULL) > 0) {
            printf("ACK received for frame %d\n", ack);
            acked[ack] = 1;
            while (base < n && acked[base]) base++;     /* slide window */
        } else {
            /* 3. Timeout: resend ONLY the un-ACKed frames */
            printf("TIMEOUT!\n");
            for (i = base; i < next; i++) {
                if (!acked[i]) {
                    f.seq = i;
                    f.data = word[i];
                    f.total = n;
                    sendto(sock, &f, sizeof(f), 0, (struct sockaddr *)&receiver, sizeof(receiver));
                    printf("Resent frame %d ('%c')\n", i, f.data);
                }
            }
        }
    }

    printf("\nAll frames sent successfully.\n");
    close(sock);
    return 0;
}

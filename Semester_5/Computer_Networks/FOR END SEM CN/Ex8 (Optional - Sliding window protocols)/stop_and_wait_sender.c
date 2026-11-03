#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

struct Frame {
    int seq;        /* 0 or 1 */
    char data;      /* one character per frame */
    int total;      /* total number of frames */
};

int main() {
    int sock, i, n, lose;
    char word[100];
    struct sockaddr_in receiver;
    struct Frame f;
    int ack;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(9000);
    receiver.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* timeout for waiting ACK = 2 seconds */
    struct timeval tv = {2, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    printf("Enter word: ");
    scanf("%s", word);
    n = strlen(word);
    printf("Frame number to lose once (0 = none): ");
    scanf("%d", &lose);

    for (i = 0; i < n; i++) {
        f.seq = i % 2;
        f.data = word[i];
        f.total = n;

        int firstTry = 1;
        while (1) {
            if (firstTry && lose == i + 1) {
                printf("Frame %d (seq %d, '%c') LOST (not sent)\n", i + 1, f.seq, f.data);
            } else {
                sendto(sock, &f, sizeof(f), 0, (struct sockaddr *)&receiver, sizeof(receiver));
                printf("Sent frame %d (seq %d, '%c')\n", i + 1, f.seq, f.data);
            }
            firstTry = 0;

            /* wait for ACK; ACK number = next expected seq */
            if (recvfrom(sock, &ack, sizeof(ack), 0, NULL, NULL) > 0 && ack == (f.seq + 1) % 2) {
                printf("ACK %d received\n\n", ack);
                break;                      /* go to next frame */
            }
            printf("TIMEOUT! Resending frame %d\n", i + 1);
        }
    }

    printf("All frames sent successfully.\n");
    close(sock);
    return 0;
}

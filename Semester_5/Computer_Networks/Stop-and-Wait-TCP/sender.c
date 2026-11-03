#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main(void)
{
    int s, total, frame = 1, ack, ready;
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Number of frames: ");
    scanf("%d", &total);

    while (frame <= total)
    {
        printf("Sending frame %d\n", frame);
        send(s, &frame, sizeof(frame), 0);

        /* Prepare to wait for an ACK for three seconds. */
        fd_set sockets;
        struct timeval timeout = {0};

        FD_ZERO(&sockets);
        FD_SET(s, &sockets);
        timeout.tv_sec = 3;

        ready = select(s + 1, &sockets, NULL, NULL, &timeout);

        if (ready == 0)
        {
            printf("Timeout! Resending frame\n");
            continue;
        }

        if (ready < 0)
        {
            close(s);
            return 1;
        }

        if (recv(s, &ack, sizeof(ack), MSG_WAITALL)
            != sizeof(ack))
        {
            close(s);
            return 1;
        }

        if (ack == frame)
        {
            printf("ACK %d received\n", ack);
            frame++;
        }
    }

    /* Frame 0 tells the receiver to stop. */
    frame = 0;
    send(s, &frame, sizeof(frame), 0);

    close(s);
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    int s, c, frame, ack;
    int expected = 1, dropped = 0;
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 1);
    c = accept(s, NULL, NULL);

    while (1)
    {
        if (recv(c, &frame, sizeof(frame), MSG_WAITALL)
            != sizeof(frame))
        {
            break;
        }

        if (frame == 0)
        {
            break;
        }

        /* Ignore frame 2 once, without sending an ACK. */
        if (frame == 2 && dropped == 0)
        {
            printf("Lost frame 2\n");
            dropped = 1;
            continue;
        }

        if (frame == expected)
        {
            printf("Accepted frame %d\n", frame);
            expected++;
        }
        else
        {
            printf("Duplicate frame discarded\n");
        }

        /* ACK the last accepted frame, including for duplicates. */
        ack = expected - 1;
        send(c, &ack, sizeof(ack), 0);
    }

    close(c);
    close(s);
    return 0;
}
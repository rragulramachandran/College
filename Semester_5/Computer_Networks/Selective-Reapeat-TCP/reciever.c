#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    int s, c, count, frame, i, ack;
    int expected = 1, dropped = 0;
    int received[101] = {0};
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
        if (recv(c, &count, sizeof(count), MSG_WAITALL)
            != sizeof(count))
        {
            break;
        }

        if (count == 0)
        {
            break;
        }

        for (i = 0; i < count; i++)
        {
            if (recv(c, &frame, sizeof(frame), MSG_WAITALL)
                != sizeof(frame))
            {
                close(c);
                close(s);
                return 1;
            }

            ack = 0;

            if (frame == 2 && dropped == 0)
            {
                printf("Lost frame 2\n");
                dropped = 1;
            }
            else if (frame >= 1 && frame <= 100)
            {
                /* Remember the frame even if an earlier one is missing. */
                received[frame] = 1;
                printf("Stored frame %d\n", frame);

                /* ACK only this particular frame. */
                ack = frame;

                /* Deliver stored frames in the correct order. */
                while (expected <= 100 && received[expected] == 1)
                {
                    printf("Delivered %d\n", expected);
                    expected++;
                }
            }

            send(c, &ack, sizeof(ack), 0);
        }
    }

    close(c);
    close(s);
    return 0;
}
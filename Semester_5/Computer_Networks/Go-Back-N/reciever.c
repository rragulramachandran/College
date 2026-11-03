#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    int s, c, count, frame, i;
    int expected = 1, dropped = 0, ack;
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
        if (recv(c, &count, sizeof(count), 0)
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

            /* Drop frame 2 only on its first attempt. */
            if (frame == 2 && dropped == 0)
            {
                printf("Lost frame 2\n");
                dropped = 1;
            }
            else if (frame == expected)
            {
                printf("Accepted %d\n", frame);
                expected++;
            }
            else
            {
                printf("Discarded %d\n", frame);
            }

            /* Acknowledge the last in-order frame. */
            ack = expected - 1;
            send(c, &ack, sizeof(ack), 0);
        }
    }

    close(c);
    close(s);
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    int s, total, base = 1, end, count, i, ack;
    int window = 3;
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Number of frames (1 to 100): ");
    scanf("%d", &total);

    while (base <= total)
    {
        /* Choose the current window. */
        end = base + window - 1;

        if (end > total)
        {
            end = total;
        }

        /* Tell the receiver how many frames follow. */
        count = end - base + 1;
        send(s, &count, sizeof(count), 0);

        /* Send all frames in the window. */
        for (i = base; i <= end; i++)
        {
            printf("Sending %d\n", i);
            send(s, &i, sizeof(i), 0);
        }

        /* Read one cumulative ACK per attempt. */
        for (i = 0; i < count; i++)
        {
            if (recv(s, &ack, sizeof(ack), MSG_WAITALL)
                != sizeof(ack))
            {
                close(s);
                return 1;
            }

            printf("ACK %d\n", ack);
        }

        /* ACK is the last frame accepted.
           Retry from the next frame. */
        base = ack + 1;
    }

    /* Zero count means transmission is finished. */
    count = 0;
    send(s, &count, sizeof(count), 0);

    close(s);
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    int s, total, base = 1, end, count, i, ack;
    int window = 3;
    int acknowledged[101] = {0};
    struct sockaddr_in server = {0};

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Number of frames (1 to 100): ");
    scanf("%d", &total);

    if (total < 1 || total > 100)
    {
        close(s);
        return 1;
    }

    while (base <= total)
    {
        end = base + window - 1;

        if (end > total)
        {
            end = total;
        }

        /* Count only frames that still need an ACK. */
        count = 0;

        for (i = base; i <= end; i++)
        {
            if (acknowledged[i] == 0)
            {
                count++;
            }
        }

        send(s, &count, sizeof(count), 0);

        /* Skip frames already acknowledged. */
        for (i = base; i <= end; i++)
        {
            if (acknowledged[i] == 0)
            {
                printf("Sending %d\n", i);
                send(s, &i, sizeof(i), 0);
            }
        }

        /* Each positive ACK acknowledges just one frame. */
        for (i = 0; i < count; i++)
        {
            if (recv(s, &ack, sizeof(ack), MSG_WAITALL)
                != sizeof(ack))
            {
                close(s);
                return 1;
            }

            if (ack >= 1 && ack <= total)
            {
                printf("ACK %d\n", ack);
                acknowledged[ack] = 1;
            }
        }

        /* Move past consecutive acknowledged frames. */
        while (base <= total && acknowledged[base] == 1)
        {
            base++;
        }
    }

    count = 0;
    send(s, &count, sizeof(count), 0);

    close(s);
    return 0;
}
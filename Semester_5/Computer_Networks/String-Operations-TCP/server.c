
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

struct String
{
    char str[100];
    int choice;
    char result[100];
};

void operation(struct String *data)
{
    int n = strlen(data->str);

    switch (data->choice)
    {
        case 1:
            sprintf(data->result, "%d", n);
            break;

        case 2:
            for (int i = 0; i < n; i++)
                data->result[i] = data->str[n - i - 1];

            data->result[n] = '\0';
            break;

        case 3:
            strcpy(data->result, data->str);

            for (int i = 0; i < n; i++)
                data->result[i] = toupper((unsigned char)data->result[i]);
            break;

        case 4:
        {
            int flag = 1;

            for (int i = 0; i < n / 2; i++)
            {
                if (data->str[i] != data->str[n - i - 1])
                {
                    flag = 0;
                    break;
                }
            }

            strcpy(data->result, flag ? "Palindrome" : "Not Palindrome");
            break;
        }

        default:
            strcpy(data->result, "Invalid Choice");
    }
}

int main()
{
    int s, c;
    struct sockaddr_in server, client;
    struct String data;
    socklen_t clientLen = sizeof(client);

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);

    while (1)
    {
        clientLen = sizeof(client);
        c = accept(s, (struct sockaddr *)&client, &clientLen);

        if (c < 0)
            continue;

        int n = recv(c, &data, sizeof(data), MSG_WAITALL);

        if (n == sizeof(data))
        {
            operation(&data);

            printf("String : %s\n", data.str);
            printf("Result : %s\n", data.result);

            send(c, &data, sizeof(data), 0);
        }

        close(c);
    }

    close(s);
    return 0;
}

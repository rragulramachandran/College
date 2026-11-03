#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE 1024

void reverse(const char *text, char *result)
{
    size_t length = strlen(text);
    for (size_t i = 0; i < length; i++)
        result[i] = text[length - i - 1];
    result[length] = '\0';
}

/* Delete the first occurrence of part from text. */
void delete(const char *text, const char *part, char *result)
{
    const char *position = strstr(text, part);
    if (position == NULL)
    {
        strcpy(result, text);
        return;
    }

    size_t prefix = (size_t)(position - text);
    memcpy(result, text, prefix);
    strcpy(result + prefix, position + strlen(part));
}

void concat(const char *first, const char *second, char *result)
{
    strcpy(result, first);
    strcat(result, second);
}

int main(void)
{
    int s;
    char request[SIZE], result[SIZE], response[SIZE + 32];
    struct sockaddr_in server = {0}, client;
    socklen_t len;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) { perror("socket"); return 1; }

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind");
        close(s);
        return 1;
    }

    printf("UDP String RPC Server waiting...\n");

    while (1)
    {
        len = sizeof(client);
        ssize_t received = recvfrom(s, request, sizeof(request) - 1, 0,
                                    (struct sockaddr *)&client, &len);
        if (received < 0) { perror("recvfrom"); continue; }
        request[received] = '\0';

        char *name = strtok(request, " \t\r\n");
        int operation = 0;

        /* Match a supported function name before validating arguments. */
        if (name != NULL)
        {
            if (strcmp(name, "reverse") == 0) operation = 1;
            else if (strcmp(name, "delete") == 0) operation = 2;
            else if (strcmp(name, "concat") == 0) operation = 3;
        }

        if (operation == 0)
            snprintf(response, sizeof(response),
                     "Error: Unknown function. Use reverse, delete, or concat.");
        else
        {
            char *first = strtok(NULL, " \t\r\n");
            char *second = strtok(NULL, " \t\r\n");
            char *extra = strtok(NULL, " \t\r\n");

            if (operation == 1 && (first == NULL || second != NULL))
                snprintf(response, sizeof(response),
                         "Error: Expected reverse <string>.");
            else if (operation != 1 &&
                     (first == NULL || second == NULL || extra != NULL))
                snprintf(response, sizeof(response),
                         "Error: Expected %s <string> <string>.", name);
            else
            {
                /* Tokens share one SIZE-byte request, so the result fits. */
                switch (operation)
                {
                    case 1: reverse(first, result); break;
                    case 2: delete(first, second, result); break;
                    case 3: concat(first, second, result); break;
                }
                snprintf(response, sizeof(response), "Result = %s", result);
            }
        }

        if (sendto(s, response, strlen(response), 0,
                   (struct sockaddr *)&client, len) < 0)
            perror("sendto");
    }

    close(s);
    return 0;
}

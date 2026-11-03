#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <arpa/inet.h>

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int div(int a, int b) { return a / b; }


int parse_int(const char *text, int *value)
{
    int negative = 0;
    unsigned long long number = 0;
    unsigned long long limit = INT_MAX;

    if (*text == '+' || *text == '-')
        negative = (*text++ == '-');
    if (negative)
        limit = (unsigned long long)(-(INT_MIN + 1)) + 1;
    if (*text == '\0')
        return 0;

    while (*text)
    {
        if (*text < '0' || *text > '9')
            return 0;
        unsigned int digit = (unsigned int)(*text++ - '0');
        if (number > (limit - digit) / 10)
            return 0;
        number = number * 10 + digit;
    }

    *value = negative ? (int)(-(long long)number) : (int)number;
    return 1;
}

int main(void)
{
    int s;
    char request[1024], response[128];
    struct sockaddr_in server = {0}, client;
    socklen_t len;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) { perror("socket"); return 1; }

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind");
        close(s);
        return 1;
    }

    printf("UDP RPC Server waiting...\n");

    while (1)
    {
        len = sizeof(client);
        ssize_t received = recvfrom(s, request, sizeof(request) - 1, 0,
                                    (struct sockaddr *)&client, &len);
        if (received < 0) { perror("recvfrom"); continue; }
        request[received] = '\0';

        char *name = strtok(request, " \t\r\n");
        int (*function)(int, int) = NULL;
        if (name != NULL)
        {
            if (strcmp(name, "add") == 0) function = add;
            else if (strcmp(name, "sub") == 0) function = sub;
            else if (strcmp(name, "mul") == 0) function = mul;
            else if (strcmp(name, "div") == 0) function = div;
        }

        /* Validate the function name before checking its arguments. */
        if (function == NULL)
            snprintf(response, sizeof(response),
                     "Error: Unknown function. Use add, sub, mul, or div.");
        else
        {
            char *first = strtok(NULL, " \t\r\n");
            char *second = strtok(NULL, " \t\r\n");
            char *extra = strtok(NULL, " \t\r\n");
            int a, b;
            if (first == NULL || second == NULL || extra != NULL)
                snprintf(response, sizeof(response),
                         "Error: Expected <function> <integer> <integer>.");
            else if (!parse_int(first, &a) || !parse_int(second, &b))
                snprintf(response, sizeof(response),
                         "Error: Datatype incompatibility. Both arguments must be integers in int range.");
            else if (function == div && b == 0)
                snprintf(response, sizeof(response), "Error: Division by zero.");
            else
            {
                long long checked;
                if (function == add) checked = (long long)a + b;
                else if (function == sub) checked = (long long)a - b;
                else if (function == mul) checked = (long long)a * b;
                else checked = (long long)a / b;

                if (checked < INT_MIN || checked > INT_MAX)
                    snprintf(response, sizeof(response), "Error: Integer overflow.");
                else
                    snprintf(response, sizeof(response), "Result = %d", function(a, b));
            }
        }

        if (sendto(s, response, strlen(response), 0,
                   (struct sockaddr *)&client, len) < 0)
            perror("sendto");
    }

    close(s);
    return 0;
}

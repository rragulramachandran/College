
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int s, n;
    char roll[20], result[100];
    struct sockaddr_in addr;

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Connect failed");
        close(s);
        return 1;
    }

    printf("Enter roll number: ");
    scanf("%19s", roll);

    send(s, roll, strlen(roll), 0);

    n = read(s, result, sizeof(result) - 1);

    if (n <= 0)
    {
        printf("Failed to receive student details\n");
        close(s);
        return 1;
    }

    result[n] = '\0';

    printf("Student Details: %s\n", result);

    close(s);

    return 0;
}

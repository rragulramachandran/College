#include <stdio.h>
#include <string.h>

int main()
{
    char data[1000], received[1000];
    int count = 0, parity;

    printf("Enter binary data: ");
    scanf("%998s", data);

    for (int i = 0; i < strlen(data); i++)
    {
        if (data[i] == '1')
            count++;
    }

    parity = count % 2;

    printf("Transmitted data: %s%d\n", data, parity);

    printf("Enter received data: ");
    scanf("%999s", received);

    count = 0;

    for (int i = 0; i < strlen(received); i++)
    {
        if (received[i] == '1')
            count++;
    }

    if (count % 2 == 0)
        printf("No error detected\n");
    else
        printf("Error detected\n");

    return 0;
}

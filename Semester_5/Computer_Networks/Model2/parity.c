#include <stdio.h>
#include <string.h>

int main()
{
    char data[100];
    int i, count = 0, parity;

    printf("Enter binary data: ");
    scanf("%s", data);

    for(i = 0; i < strlen(data); i++)
    {
        if(data[i] == '1')
            count++;
    }

    parity = count % 2;

    printf("Transmitted data: %s%d\n", data, parity);

    // Receiver
    count = 0;

    for(i = 0; i < strlen(data); i++)
    {
        if(data[i] == '1')
            count++;
    }

    count += parity;

    if(count % 2 == 0)
        printf("No error detected\n");
    else
        printf("Error detected\n");

    return 0;
}
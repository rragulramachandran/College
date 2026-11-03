#include <stdio.h>
#include <string.h>

#define SIZE 1000
#define FLAG '~'
#define ESC '}'

void byteStuffing(char data[], char stuffed[])
{
    int i, j = 0;

    stuffed[j++] = FLAG;

    for (i = 0; data[i] != '\0'; i++)
    {
        if (data[i] == FLAG || data[i] == ESC)
        {
            stuffed[j++] = ESC;
        }

        stuffed[j++] = data[i];
    }

    stuffed[j++] = FLAG;
    stuffed[j] = '\0';
}

void byteDestuffing(char stuffed[], char original[])
{
    int i, j = 0;
    int len = strlen(stuffed);

    for (i = 1; i < len - 1; i++)
    {
        if (stuffed[i] == ESC)
        {
            i++;
        }

        original[j++] = stuffed[i];
    }

    original[j] = '\0';
}

int main()
{
    char data[SIZE];
    char stuffed[SIZE * 2], original[SIZE * 2];

    printf("Enter data: ");
    fgets(data, SIZE, stdin);
    data[strcspn(data, "\n")] = '\0';

    byteStuffing(data, stuffed);
    printf("After Byte Stuffing: %s\n", stuffed);

    byteDestuffing(stuffed, original);
    printf("After Byte Destuffing: %s\n", original);

    return 0;
}

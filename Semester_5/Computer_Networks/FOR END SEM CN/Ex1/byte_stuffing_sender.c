#include <stdio.h>
#include <string.h>

#define SIZE 1000
#define FLAG '~'
#define ESC '}'

int main()
{
    char data[SIZE], stuffed[SIZE * 2];
    int i, j = 0;

    printf("Sender - Enter data: ");
    fgets(data, SIZE, stdin);
    data[strcspn(data, "\n")] = '\0';

    stuffed[j++] = FLAG;

    for (i = 0; data[i] != '\0'; i++)
    {
        if (data[i] == FLAG || data[i] == ESC)
            stuffed[j++] = ESC;

        stuffed[j++] = data[i];
    }

    stuffed[j++] = FLAG;
    stuffed[j] = '\0';

    printf("Stuffed frame sent: %s\n", stuffed);
    return 0;
}

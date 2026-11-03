#include <stdio.h>
#include <string.h>

#define FLAG 0x7E
#define ESC  0x7D

int main()
{
    unsigned char data[100], frame[200], out[100];
    int i, j = 0, k = 0, n;

    printf("Enter data: ");
    scanf("%99s", data);
    n = strlen((char *)data);

    frame[j++] = FLAG;
    frame[j++] = 0xFF;
    frame[j++] = 0x03;
    frame[j++] = 0x00;
    frame[j++] = 0x21;

    for (i = 0; i < n; i++)
    {
        if (data[i] == FLAG || data[i] == ESC)
        {
            frame[j++] = ESC;
            frame[j++] = data[i];
        }
        else
            frame[j++] = data[i];
    }

    frame[j++] = FLAG;

    printf("Stuffed PPP Frame: ");
    for (i = 0; i < j; i++)
        printf("%02X ", frame[i]);

    for (i = 5; i < j - 1; i++)
    {
        if (frame[i] == ESC)
            out[k++] = frame[++i];
        else
            out[k++] = frame[i];
    }

    out[k] = '\0';

    printf("\nDestuffed data (Hex): ");
    for (i = 0; i < k; i++)
        printf("%02X ", out[i]);

    printf("\nDestuffed data: %s\n", out);

    return 0;
}
#include <stdio.h>
#include <string.h>

#define SIZE 2000
#define FLAG '~'
#define ESC '}'

int main()
{
    char stuffed[SIZE], original[SIZE];
    int i, j = 0, len;

    printf("Receiver - Enter stuffed frame: ");
    fgets(stuffed, SIZE, stdin);
    stuffed[strcspn(stuffed, "\n")] = '\0';

    len = strlen(stuffed);

    if (len < 2 || stuffed[0] != FLAG || stuffed[len - 1] != FLAG)
    {
        printf("Invalid frame: start/end flag missing.\n");
        return 1;
    }

    for (i = 1; i < len - 1; i++)
    {
        if (stuffed[i] == ESC)
        {
            i++;
            if (i >= len - 1)
            {
                printf("Invalid frame: incomplete escape sequence.\n");
                return 1;
            }
        }

        original[j++] = stuffed[i];
    }

    original[j] = '\0';
    printf("Original data received: %s\n", original);
    return 0;
}

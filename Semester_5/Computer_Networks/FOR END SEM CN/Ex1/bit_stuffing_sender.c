#include <stdio.h>
#define SIZE 1000

int main()
{
    char data[SIZE], stuffed[SIZE * 2];
    int i, j = 0, count = 0;

    printf("Sender - Enter binary data: ");
    scanf("%999s", data);

    for (i = 0; data[i] != '\0'; i++)
    {
        stuffed[j++] = data[i];

        if (data[i] == '1')
        {
            count++;

            if (count == 5)
            {
                stuffed[j++] = '0';
                count = 0;
            }
        }
        else
        {
            count = 0;
        }
    }

    stuffed[j] = '\0';
    printf("Stuffed data sent: %s\n", stuffed);

    return 0;
}

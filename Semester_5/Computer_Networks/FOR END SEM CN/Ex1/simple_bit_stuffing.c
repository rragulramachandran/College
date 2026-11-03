#include <stdio.h>
#include <string.h>

#define SIZE 1000

void bitStuffing(char data[], char stuffed[])
{
    int i, j = 0, count = 0;

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
}

void bitDestuffing(char stuffed[], char original[])
{
    int i, j = 0, count = 0;

    for (i = 0; stuffed[i] != '\0'; i++)
    {
        original[j++] = stuffed[i];

        if (stuffed[i] == '1')
        {
            count++;

            if (count == 5)
            {
                i++;
                count = 0;
            }
        }
        else
        {
            count = 0;
        }
    }

    original[j] = '\0';
}

int main()
{
    char data[SIZE];
    char stuffed[SIZE * 2], original[SIZE * 2];

    printf("Enter binary data: ");
    scanf("%999s", data);

    bitStuffing(data, stuffed);
    printf("After Bit Stuffing: %s\n", stuffed);

    bitDestuffing(stuffed, original);
    printf("After Bit Destuffing: %s\n", original);

    return 0;
}

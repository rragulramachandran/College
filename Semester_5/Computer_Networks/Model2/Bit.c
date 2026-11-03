#include <stdio.h>
#include <string.h>

int main()
{
    char data[100], bits[1000] = "";
    char frame[2000] = "", out[1000] = "";
    int i, j = 0, k = 0, ones = 0;

    printf("Enter the data: ");
    scanf("%99s", data);

    for (i = 0; data[i] != '\0'; i++)
    {
        for (int b = 7; b >= 0; b--)
        {
            bits[j++] = ((data[i] >> b) & 1) + '0';
        }
    }
    bits[j] = '\0';

    strcat(frame, "01111110");
    j = strlen(frame);

    for (i = 0; bits[i] != '\0'; i++)
    {
        frame[j++] = bits[i];

        if (bits[i] == '1')
        {
            ones++;
            if (ones == 5)
            {
                frame[j++] = '0';
                ones = 0;
            }
        }
        else
            ones = 0;
    }

    strcat(frame, "01111110");

    printf("Original Data: %s\n", data);
    printf("Original Bits: %s\n", bits);
    printf("Stuffed Frame: %s\n", frame);

    ones = 0;

    for (i = 8; i < strlen(frame) - 8; i++)
    {
        out[k++] = frame[i];

        if (frame[i] == '1')
        {
            ones++;
            if (ones == 5)
            {
                if (frame[i + 1] == '0')
                    i++;
                ones = 0;
            }
        }
        else
            ones = 0;
    }
    out[k] = '\0';

    printf("Destuffed Bits: %s\n", out);

    if (strcmp(bits, out) == 0)
        printf("Verification: SUCCESS\n");
    else
        printf("Verification: FAILED\n");

    return 0;
}

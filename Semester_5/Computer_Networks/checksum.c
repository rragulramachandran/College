#include <stdio.h>
#include <string.h>
#include <math.h>

int main()
{
    char str[100];
    int binary[100][8];
    int sum = 0, checksum, receiverSum = 0;

    printf("Enter a string: ");
    fgets(str, sizeof(str), stdin);
    str[strcspn(str, "\n")] = '\0';

    int n = strlen(str);

    printf("\nBinary Data Words:\n");

    for (int i = 0; i < n; i++)
    {
        int num = (unsigned char)str[i];

        for (int j = 7; j >= 0; j--)
        {
            binary[i][j] = num % 2;
            num = num / 2;
        }

        for (int j = 0; j < 8; j++)
            printf("%d", binary[i][j]);

        printf("\n");
    }

    // Sender Side

    for (int i = 0; i < n; i++)
    {
        int value = 0;

        for (int j = 0; j < 8; j++)
            value += binary[i][j] * (int)pow(2, 7 - j);

        sum += value;

        while (sum > 255)
            sum = (sum % 256) + (sum / 256);
    }

    checksum = 255 - sum;

    printf("\nSender Checksum: ");

    for (int i = 7; i >= 0; i--)
        printf("%d", (checksum / (int)pow(2, i)) % 2);

    printf("\n");

    // Receiver Side

    for (int i = 0; i < n; i++)
    {
        int value = 0;

        for (int j = 0; j < 8; j++)
            value += binary[i][j] * (int)pow(2, 7 - j);

        receiverSum += value;

        while (receiverSum > 255)
            receiverSum = (receiverSum % 256) + (receiverSum / 256);
    }

    receiverSum += checksum;

    while (receiverSum > 255)
        receiverSum = (receiverSum % 256) + (receiverSum / 256);

    printf("\nReceiver Sum: ");

    for (int i = 7; i >= 0; i--)
        printf("%d", (receiverSum / (int)pow(2, i)) % 2);

    if (receiverSum == 255)
        printf("\nNo Error Detected\n");
    else
        printf("\nError Detected\n");

    return 0;
}
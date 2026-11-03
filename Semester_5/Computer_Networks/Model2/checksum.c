#include <stdio.h>

int main()
{
    int n, i, j, sum = 0, checksum, value;
    char data[20][9];

    printf("Enter number of data words: ");
    scanf("%d", &n);

    for(i = 0; i < n; i++)
    {
        printf("Enter 8-bit binary data: ");
        scanf("%8s", data[i]);

        value = 0;
        for(j = 0; j < 8; j++)
            value = value * 2 + (data[i][j] - '0');

        sum += value;

        while(sum > 255)
            sum = (sum & 255) + (sum >> 8);
    }

    checksum = (~sum) & 255;

    printf("Checksum: ");
    for(i = 7; i >= 0; i--)
        printf("%d", (checksum >> i) & 1);

    // Receiver
    sum = checksum;

    for(i = 0; i < n; i++)
    {
        value = 0;
        for(j = 0; j < 8; j++)
            value = value * 2 + (data[i][j] - '0');

        sum += value;

        while(sum > 255)
            sum = (sum & 255) + (sum >> 8);
    }

    if(sum == 255)
        printf("\nNo error detected\n");
    else
        printf("\nError detected\n");

    return 0;
}
#include <stdio.h>
#include <string.h>

int calculateChecksum(char data[])
{
    int i, sum = 0;

    for (i = 0; data[i] != '\0'; i++)
        sum += (unsigned char)data[i];

    return (~sum) & 255;
}

int main()
{
    char data[200];
    int checksum;

    printf("Sender - Enter data: ");
    fgets(data, sizeof(data), stdin);
    data[strcspn(data, "\n")] = '\0';

    checksum = calculateChecksum(data);

    printf("Data: %s\n", data);
    printf("Checksum (8-bit): %02X\n", checksum);
    printf("Send the data and checksum to the receiver.\n");

    return 0;
}

#include <stdio.h>
#include <string.h>

int main()
{
    char data[200];
    int i, sum = 0, checksum;

    printf("Enter data: ");
    fgets(data, sizeof(data), stdin);

    data[strcspn(data, "\n")] = '\0';

    for (i = 0; data[i] != '\0'; i++)
        sum += (unsigned char)data[i];

    checksum = (~sum) & 255;

    printf("Checksum: %02X\n", checksum);

    return 0;
}

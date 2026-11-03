#include <stdio.h>
#include <string.h>

int main()
{
    char data[200];
    int receivedChecksum, sum = 0, i;

    printf("Receiver - Enter received data: ");
    fgets(data, sizeof(data), stdin);
    data[strcspn(data, "\n")] = '\0';

    printf("Enter received checksum in decimal (0-255): ");
    scanf("%d", &receivedChecksum);

    for (i = 0; data[i] != '\0'; i++)
        sum += (unsigned char)data[i];

    sum += receivedChecksum;

    if ((sum & 255) == 255)
        printf("Checksum valid. No error detected.\n");
    else
        printf("Checksum invalid. Error detected.\n");

    return 0;
}

#include <stdio.h>
#include <string.h>

void divideCRC(char data[], char generator[], char remainder[])
{
    char work[500];
    int i, j, dataLen = strlen(data), genLen = strlen(generator);

    strcpy(work, data);

    for (i = 0; i <= dataLen - genLen; i++)
    {
        if (work[i] == '1')
        {
            for (j = 0; j < genLen; j++)
                work[i + j] = (work[i + j] == generator[j]) ? '0' : '1';
        }
    }

    strcpy(remainder, work + dataLen - (genLen - 1));
    remainder[genLen - 1] = '\0';
}

int main()
{
    char data[300], generator[100], augmented[500];
    char crc[100], codeword[500];
    int i, dataLen, crcLen;

    printf("Sender - Enter binary dataword: ");
    scanf("%299s", data);

    printf("Enter generator bits (example 10011): ");
    scanf("%99s", generator);

    dataLen = strlen(data);
    crcLen = strlen(generator) - 1;

    if (strlen(generator) < 2 || generator[0] != '1')
    {
        printf("Invalid generator.\n");
        return 1;
    }

    strcpy(augmented, data);
    for (i = 0; i < crcLen; i++)
        strcat(augmented, "0");

    divideCRC(augmented, generator, crc);

    strcpy(codeword, data);
    strcat(codeword, crc);

    printf("CRC remainder: %s\n", crc);
    printf("Codeword to send: %s\n", codeword);

    return 0;
}

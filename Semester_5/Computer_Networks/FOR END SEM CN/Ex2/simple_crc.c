#include <stdio.h>
#include <string.h>

int main()
{
    char data[300], generator[100], work[500], crc[100];
    int i, j, dataLen, genLen;

    printf("Enter binary data: ");
    scanf("%299s", data);

    printf("Enter generator bits (example 10011): ");
    scanf("%99s", generator);

    dataLen = strlen(data);
    genLen = strlen(generator);

    if (genLen < 2 || generator[0] != '1')
    {
        printf("Invalid generator.\n");
        return 1;
    }

    strcpy(work, data);

    for (i = 0; i < genLen - 1; i++)
        strcat(work, "0");

    for (i = 0; i <= (int)strlen(work) - genLen; i++)
    {
        if (work[i] == '1')
        {
            for (j = 0; j < genLen; j++)
            {
                if (work[i + j] == generator[j])
                    work[i + j] = '0';
                else
                    work[i + j] = '1';
            }
        }
    }

    strcpy(crc, work + dataLen);
    crc[genLen - 1] = '\0';

    printf("CRC remainder: %s\n", crc);

    return 0;
}

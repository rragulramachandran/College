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
    char codeword[500], generator[100], remainder[100];
    int i, error = 0;

    printf("Receiver - Enter received codeword: ");
    scanf("%499s", codeword);

    printf("Enter the same generator bits: ");
    scanf("%99s", generator);

    if (strlen(generator) < 2 || generator[0] != '1' ||
        strlen(codeword) < strlen(generator))
    {
        printf("Invalid input.\n");
        return 1;
    }

    divideCRC(codeword, generator, remainder);

    printf("Remainder: %s\n", remainder);

    for (i = 0; remainder[i] != '\0'; i++)
    {
        if (remainder[i] != '0')
        {
            error = 1;
            break;
        }
    }

    if (error == 0)
    {
        codeword[strlen(codeword) - (strlen(generator) - 1)] = '\0';
        printf("No error detected.\n");
        printf("Recovered dataword: %s\n", codeword);
    }
    else
    {
        printf("Error detected. Data may be corrupted.\n");
    }

    return 0;
}

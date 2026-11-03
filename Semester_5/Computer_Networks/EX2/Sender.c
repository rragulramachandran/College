#include <stdio.h>
#include <string.h>
#include "header.h"

void modulo2Division(const char *dividend, const char *divisor, char *remainder)
{
    int dividendLen = strlen(dividend);
    int divisorLen = strlen(divisor);

    char work[MAX_LEN];
    strcpy(work, dividend);

    for (int i = 0; i <= dividendLen - divisorLen; i++)
    {
        if (work[i] == '1')
        {
            for (int j = 0; j < divisorLen; j++)
            {
                work[i + j] = (work[i + j] == divisor[j]) ? '0' : '1';
            }
        }
    }

    strcpy(remainder, work + (dividendLen - (divisorLen - 1)));
}

void generateCRC(const char *data, const char *generator, char *codeword)
{
    int dataLen = strlen(data);
    int genLen = strlen(generator);
    int r = genLen - 1;

    char paddedData[MAX_LEN];
    strcpy(paddedData, data);
    for (int i = 0; i < r; i++)
    {
        paddedData[dataLen + i] = '0';
    }
    paddedData[dataLen + r] = '\0';

    char remainder[MAX_LEN];
    modulo2Division(paddedData, generator, remainder);

    strcpy(codeword, data);
    strcat(codeword, remainder);

    printf("\n[Sender] Dataword           : %s\n", data);
    printf("[Sender] Generator (g(x))   : %s  (r = %d CRC bits)\n", generator, r);
    printf("[Sender] Dataword padded with %d zeros : %s\n", r, paddedData);
    printf("[Sender] Remainder (CRC bits)          : %s\n", remainder);
    printf("[Sender] Codeword to transmit (data+CRC): %s\n", codeword);
}

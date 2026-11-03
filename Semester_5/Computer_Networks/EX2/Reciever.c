#include <stdio.h>
#include <string.h>
#include "header.h"

int checkCRC(const char *codeword, const char *generator, char *syndrome)
{
    modulo2Division(codeword, generator, syndrome);

    int r = strlen(syndrome);
    int isZero = 1;
    for (int i = 0; i < r; i++)
    {
        if (syndrome[i] != '0')
        {
            isZero = 0;
            break;
        }
    }

    printf("\n[Receiver] Received codeword : %s\n", codeword);
    printf("[Receiver] Generator (g(x))  : %s\n", generator);
    printf("[Receiver] Syndrome (remainder) : %s\n", syndrome);

    if (isZero)
    {
        printf("[Receiver] Syndrome is ZERO -> no error detected. Dataword accepted.\n");
    }
    else
    {
        printf("[Receiver] Syndrome is NONZERO -> error detected. Dataword discarded.\n");
    }

    return isZero;
}

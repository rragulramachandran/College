#include <stdio.h>
#include "hamming.h"

int detectAndCorrectError(int received[], int n)
{
    int errorPosition = 0;

    int r = 0;
    while ((1 << r) <= n) r++;

    for (int i = 0; i < r; i++)
    {
        int p = 1 << i;
        if (p > n) break;

        int count = 0;
        for (int pos = 1; pos <= n; pos++)
        {
            if ((pos & p) != 0)
            {
                count += received[pos];
            }
        }

        if (count % 2 != 0)
        {
            errorPosition += p;
        }
    }

    if (errorPosition == 0)
    {
        printf("[Receiver] No error detected.\n");
    }
    else if (errorPosition > n)
    {
        printf("[Receiver] Multiple/unrecoverable error detected (syndrome=%d).\n",
               errorPosition);
    }
    else
    {
        printf("[Receiver] Error detected at bit position %d. Correcting it...\n",
               errorPosition);
        received[errorPosition] = received[errorPosition] ^ 1;

        printf("[Receiver] Corrected Hamming Code: ");
        for (int i = 1; i <= n; i++)
        {
            printf("%d", received[i]);
        }
        printf("\n");
    }

    return errorPosition;
}

void extractData(int hammingCode[], int n, int data[], int m)
{
    int j = 0;
    for (int i = 1; i <= n && j < m; i++)
    {
        if (!isPowerOfTwo(i))
        {
            data[j++] = hammingCode[i];
        }
    }

    printf("[Receiver] Extracted original data bits: ");
    for (int i = 0; i < m; i++)
    {
        printf("%d", data[i]);
    }
    printf("\n");
}
#include <stdio.h>
#include "hamming.h"


int main()
{
    int data[MAX_BITS];
    int hammingCode[MAX_BITS];
    int recoveredData[MAX_BITS];
    int m, n;
    int errorBitPosition;

    printf("Enter number of data bits: ");
    scanf("%d", &m);

    printf("Enter the %d data bits (0/1), one by one:\n", m);
    for (int i = 0; i < m; i++)
    {
        scanf("%d", &data[i]);
    }

    generateHammingCode(data, m, hammingCode, &n);

    printf("\nEnter the bit position (1 to %d) to flip and simulate a transmission "
           "error (0 for no error): ", n);
    scanf("%d", &errorBitPosition);

    if (errorBitPosition >= 1 && errorBitPosition <= n)
    {
        hammingCode[errorBitPosition] ^= 1;
        printf("[Channel] Bit %d flipped during transmission.\n", errorBitPosition);
        printf("[Channel] Transmitted (corrupted) code: ");
        for (int i = 1; i <= n; i++)
        {
            printf("%d", hammingCode[i]);
        }
        printf("\n");
    }
    else
    {
        printf("[Channel] No error introduced. Code transmitted as-is.\n");
    }

    printf("\n--- Receiver Processing ---\n");
    detectAndCorrectError(hammingCode, n);
    extractData(hammingCode, n, recoveredData, m);

    int match = 1;
    for (int i = 0; i < m; i++)
    {
        if (data[i] != recoveredData[i])
        {
            match = 0;
            break;
        }
    }

    printf("\nOriginal data bits:  ");
    for (int i = 0; i < m; i++) printf("%d", data[i]);
    printf("\nRecovered data bits: ");
    for (int i = 0; i < m; i++) printf("%d", recoveredData[i]);

    printf("\n\nResult: %s\n", match ? "SUCCESS - data matches original!"
                                      : "FAILURE - data does not match!");

    return 0;
}
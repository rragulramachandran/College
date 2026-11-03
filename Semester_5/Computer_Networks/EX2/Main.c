#include <stdio.h>
#include <string.h>
#include "header.h"

int main()
{
    char generator[MAX_LEN];
    char data[MAX_LEN];
    char codeword[MAX_LEN];
    char syndrome[MAX_LEN];

    printf("Enter the generator polynomial / key g(x) as a bit string\n");
    printf("(e.g. 1011 for x^3+x+1, i.e. CRC-3): ");
    scanf("%s", generator);

    printf("Enter the source dataword as a bit string (e.g. 1001): ");
    scanf("%s", data);

    if (strlen(generator) < 2)
    {
        printf("Error: generator must have at least 2 bits.\n");
        return 1;
    }

    generateCRC(data, generator, codeword);

    int n = strlen(codeword);

    int flipPos;
    printf("\nSimulate a transmission error? Enter a bit position to flip\n");
    printf("(1 to %d, left to right) or 0 for no error: ", n);
    scanf("%d", &flipPos);

    if (flipPos >= 1 && flipPos <= n)
    {
        codeword[flipPos - 1] = (codeword[flipPos - 1] == '0') ? '1' : '0';
        printf("[Channel] Bit %d flipped during transmission.\n", flipPos);
        printf("[Channel] Transmitted (possibly corrupted) codeword: %s\n", codeword);
    }
    else
    {
        printf("[Channel] No error introduced. Codeword transmitted as-is.\n");
    }

    int accepted = checkCRC(codeword, generator, syndrome);

    if (accepted)
    {
        int dataLen = strlen(data);
        char recoveredData[MAX_LEN];
        strncpy(recoveredData, codeword, dataLen);
        recoveredData[dataLen] = '\0';

        printf("\nRecovered dataword: %s\n", recoveredData);
        printf("Result: %s\n",
               strcmp(data, recoveredData) == 0
                   ? "SUCCESS - recovered dataword matches original!"
                   : "FAILURE - recovered dataword does not match!");
    }
    else
    {
        printf("\nResult: Dataword was discarded by the receiver due to detected error.\n");
    }

    return 0;
}

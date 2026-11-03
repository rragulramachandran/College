#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char *data = NULL, *received = NULL;

    printf("Enter binary data: ");
    scanf("%s", data);

    int m = strlen(data);

    int r = 0;

    while ((pow(2, r)) < m + r + 1)
        r++;

    int n = m + r;
    int *h = calloc(n + 1, sizeof(int));

    int j = 0, p = 0;

    for (int i = 1; i <= n; i++)
    {
        if (i == pow(2, p))
            p++;
        else
            h[i] = data[j++] - '0'; 
    }

    for (int p = 1; p <= n; p *= 2)
    {
        for (int i = 1; i <= n; i++)
        {
            if (i & p)
                h[p] ^= h[i];
        }
    }

    printf("Parity bits: %d\n", r);
    printf("Hamming Code: ");

    for (int i = 1; i <= n; i++)
        printf("%d", h[i]);

    char recieved[100];
    printf("\nEnter received code: ");
    scanf("%s", recieved);


    int error = 0;

    for (int k = 0; k < r; k++)
    {
        int p = (int)pow(2, k);
        int parity = 0;

        for (int i = 1; i <= n; i++)
        {
            if ((i / p) % 2 == 1)
                parity = (parity + (received[i - 1] - '0')) % 2;
        }

        if (parity == 1)
            error += p;
    }


    if (error == 0)
        printf("No error detected\n");
    else if (error <= n)
    {
        printf("Error at position: %d\n", error);

        received[error - 1] =
            (received[error - 1] == '0') ? '1' : '0';
    }

    printf("Corrected Code: %s\n", received);

    printf("Original Data: ");

    int p = 0;

    for (int i = 1; i <= n; i++)
    {
        if (i == pow(2, p))
            p++;
        else
            printf("%c", received[i - 1]);
}

    printf("\n");

    free(data);
    free(received);
    free(h);

    return 0;
}

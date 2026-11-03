#include <stdio.h>

/* Hamming (7,4) code
   Position:  1   2   3   4   5   6   7
   Bit:       p1  p2  d1  p4  d2  d3  d4   */

int main() {
    int d[5], h[8], i;

    printf("Enter 4 data bits (d1 d2 d3 d4): ");
    for (i = 1; i <= 4; i++) scanf("%d", &d[i]);

    /* 1. Place data bits */
    h[3] = d[1];
    h[5] = d[2];
    h[6] = d[3];
    h[7] = d[4];

    /* 2. Parity bits (even parity) */
    h[1] = h[3] ^ h[5] ^ h[7];
    h[2] = h[3] ^ h[6] ^ h[7];
    h[4] = h[5] ^ h[6] ^ h[7];

    printf("\nHamming code (positions 1-7): ");
    for (i = 1; i <= 7; i++) printf("%d ", h[i]);
    printf("\n");

    /* 3. Introduce an error (simulate noise) */
    char ch;
    printf("\nIntroduce an error? (y/n): ");
    scanf(" %c", &ch);
    if (ch == 'y') {
        int p;
        printf("Enter bit position to flip (1-7): ");
        scanf("%d", &p);
        h[p] ^= 1;
        printf("Received code: ");
        for (i = 1; i <= 7; i++) printf("%d ", h[i]);
        printf("\n");
    }

    /* 4. Recompute checks -> error position */
    int c1 = h[1] ^ h[3] ^ h[5] ^ h[7];
    int c2 = h[2] ^ h[3] ^ h[6] ^ h[7];
    int c4 = h[4] ^ h[5] ^ h[6] ^ h[7];
    int pos = c4 * 4 + c2 * 2 + c1;

    /* 5. Detect and correct */
    if (pos == 0) {
        printf("No error detected.\n");
    } else {
        printf("Error at position %d. Correcting...\n", pos);
        h[pos] ^= 1;
    }

    printf("Final data bits: %d %d %d %d\n", h[3], h[5], h[6], h[7]);
    return 0;
}

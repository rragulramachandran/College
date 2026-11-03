#include <stdio.h>

/* Hamming (7,4) code
   Position:  1   2   3   4   5   6   7
   Bit:       p1  p2  d1  p4  d2  d3  d4   */

int main() {
    int d[5], h[8], i;

    printf("Enter 4 data bits (d1 d2 d3 d4): ");
    for (i = 1; i <= 4; i++) scanf("%d", &d[i]);

    /* 1. Place data bits at non-power-of-2 positions */
    h[3] = d[1];
    h[5] = d[2];
    h[6] = d[3];
    h[7] = d[4];

    /* 2. Calculate parity bits (even parity) */
    h[1] = h[3] ^ h[5] ^ h[7];   /* checks positions 1,3,5,7 */
    h[2] = h[3] ^ h[6] ^ h[7];   /* checks positions 2,3,6,7 */
    h[4] = h[5] ^ h[6] ^ h[7];   /* checks positions 4,5,6,7 */

    /* 3. Show codeword */
    printf("\nHamming code to send (positions 1-7): ");
    for (i = 1; i <= 7; i++) printf("%d ", h[i]);
    printf("\n");

    /* 4. "Send" = write to file */
    FILE *f = fopen("channel.txt", "w");
    for (i = 1; i <= 7; i++) fprintf(f, "%d ", h[i]);
    fclose(f);

    printf("Sender: codeword sent to channel.txt\n");
    return 0;
}

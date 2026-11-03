#include <stdio.h>

int main() {
    int h[8], i;

    /* 1. Receive from file */
    FILE *f = fopen("channel.txt", "r");
    if (!f) { printf("Run the sender first!\n"); return 1; }
    for (i = 1; i <= 7; i++) fscanf(f, "%d", &h[i]);
    fclose(f);

    /* 2. Optionally introduce an error (channel noise) */
    char ch;
    printf("Introduce an error? (y/n): ");
    scanf(" %c", &ch);
    if (ch == 'y') {
        int pos;
        printf("Enter bit position to flip (1-7): ");
        scanf("%d", &pos);
        h[pos] ^= 1;
    }

    printf("Received codeword: ");
    for (i = 1; i <= 7; i++) printf("%d ", h[i]);
    printf("\n");

    /* 3. Recompute parity checks -> syndrome */
    int c1 = h[1] ^ h[3] ^ h[5] ^ h[7];
    int c2 = h[2] ^ h[3] ^ h[6] ^ h[7];
    int c4 = h[4] ^ h[5] ^ h[6] ^ h[7];
    int pos = c4 * 4 + c2 * 2 + c1;      /* error position (0 = no error) */

    /* 4. Detect and correct */
    if (pos == 0) {
        printf("No error detected.\n");
    } else {
        printf("Error at position %d. Correcting...\n", pos);
        h[pos] ^= 1;
    }

    /* 5. Extract data bits */
    printf("Original data bits: %d %d %d %d\n", h[3], h[5], h[6], h[7]);
    return 0;
}

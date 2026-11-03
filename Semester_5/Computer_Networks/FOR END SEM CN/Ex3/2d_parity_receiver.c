#include <stdio.h>

#define MAXR 20
#define C 8

int main() {
    int data[MAXR][C], rowP[MAXR], colP[C], n, i, j;

    /* 1. Receive from file */
    FILE *f = fopen("channel.txt", "r");
    if (!f) { printf("Run the sender first!\n"); return 1; }
    fscanf(f, "%d", &n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < C; j++) fscanf(f, "%d", &data[i][j]);
        fscanf(f, "%d", &rowP[i]);
    }
    for (j = 0; j < C; j++) fscanf(f, "%d", &colP[j]);
    fclose(f);

    /* 2. Optionally introduce an error (channel noise) */
    char ch;
    printf("Introduce an error? (y/n): ");
    scanf(" %c", &ch);
    if (ch == 'y') {
        int r, c;
        printf("Enter row (1-%d) and column (1-%d): ", n, C);
        scanf("%d %d", &r, &c);
        data[r - 1][c - 1] ^= 1;          /* flip the bit */
    }

    /* 3. Re-calculate parity and compare */
    int badRow = -1, badCol = -1;

    for (i = 0; i < n; i++) {
        int ones = 0;
        for (j = 0; j < C; j++) ones += data[i][j];
        if (ones % 2 != rowP[i]) badRow = i;
    }
    for (j = 0; j < C; j++) {
        int ones = 0;
        for (i = 0; i < n; i++) ones += data[i][j];
        if (ones % 2 != colP[j]) badCol = j;
    }

    /* 4. Detect and correct */
    if (badRow == -1 && badCol == -1) {
        printf("\nNo error detected.\n");
    } else if (badRow != -1 && badCol != -1) {
        printf("\nError found at row %d, column %d. Correcting...\n", badRow + 1, badCol + 1);
        data[badRow][badCol] ^= 1;
    } else {
        printf("\nError detected but cannot be located (likely a parity bit itself).\n");
    }

    /* 5. Decode message */
    printf("Received message: ");
    for (i = 0; i < n; i++) {
        int ch = 0;
        for (j = 0; j < C; j++) ch = (ch << 1) | data[i][j];
        printf("%c", ch);
    }
    printf("\n");
    return 0;
}

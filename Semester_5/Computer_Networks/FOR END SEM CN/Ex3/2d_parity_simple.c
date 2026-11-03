#include <stdio.h>
#include <string.h>

#define MAXR 20
#define C 8

int main() {
    char msg[MAXR + 1];
    int data[MAXR][C], rowP[MAXR], colP[C], n, i, j;

    printf("Enter message (max %d chars, no spaces): ", MAXR);
    scanf("%20s", msg);
    n = strlen(msg);

    /* 1. Characters -> 8 bits each */
    for (i = 0; i < n; i++)
        for (j = 0; j < C; j++)
            data[i][j] = (msg[i] >> (7 - j)) & 1;

    /* 2. Even parity for each row */
    for (i = 0; i < n; i++) {
        int ones = 0;
        for (j = 0; j < C; j++) ones += data[i][j];
        rowP[i] = ones % 2;
    }

    /* 3. Even parity for each column */
    for (j = 0; j < C; j++) {
        int ones = 0;
        for (i = 0; i < n; i++) ones += data[i][j];
        colP[j] = ones % 2;
    }

    printf("\nData with parity bits:\n");
    for (i = 0; i < n; i++) {
        printf("%c : ", msg[i]);
        for (j = 0; j < C; j++) printf("%d ", data[i][j]);
        printf("| %d\n", rowP[i]);
    }
    printf("P : ");
    for (j = 0; j < C; j++) printf("%d ", colP[j]);
    printf("\n");

    /* 4. Introduce an error (simulate noise) */
    char ch;
    printf("\nIntroduce an error? (y/n): ");
    scanf(" %c", &ch);
    if (ch == 'y') {
        int r, c;
        printf("Enter row (1-%d) and column (1-%d): ", n, C);
        scanf("%d %d", &r, &c);
        data[r - 1][c - 1] ^= 1;
    }

    /* 5. Check parity again */
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

    /* 6. Detect and correct */
    if (badRow == -1 && badCol == -1) {
        printf("\nNo error detected.\n");
    } else if (badRow != -1 && badCol != -1) {
        printf("\nError found at row %d, column %d. Correcting...\n", badRow + 1, badCol + 1);
        data[badRow][badCol] ^= 1;
    } else {
        printf("\nError detected but cannot be located.\n");
    }

    printf("Final message: ");
    for (i = 0; i < n; i++) {
        int v = 0;
        for (j = 0; j < C; j++) v = (v << 1) | data[i][j];
        printf("%c", v);
    }
    printf("\n");
    return 0;
}

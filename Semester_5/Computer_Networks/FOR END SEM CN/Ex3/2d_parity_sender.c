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

    /* 1. Convert each character to 8 bits */
    for (i = 0; i < n; i++)
        for (j = 0; j < C; j++)
            data[i][j] = (msg[i] >> (7 - j)) & 1;

    /* 2. Row parity (even parity) */
    for (i = 0; i < n; i++) {
        int ones = 0;
        for (j = 0; j < C; j++) ones += data[i][j];
        rowP[i] = ones % 2;
    }

    /* 3. Column parity (even parity) */
    for (j = 0; j < C; j++) {
        int ones = 0;
        for (i = 0; i < n; i++) ones += data[i][j];
        colP[j] = ones % 2;
    }

    /* 4. Show the matrix */
    printf("\nData with parity bits:\n");
    for (i = 0; i < n; i++) {
        printf("%c : ", msg[i]);
        for (j = 0; j < C; j++) printf("%d ", data[i][j]);
        printf("| %d\n", rowP[i]);
    }
    printf("P : ");
    for (j = 0; j < C; j++) printf("%d ", colP[j]);
    printf("\n");

    /* 5. "Send" = write to file */
    FILE *f = fopen("channel.txt", "w");
    fprintf(f, "%d\n", n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < C; j++) fprintf(f, "%d ", data[i][j]);
        fprintf(f, "%d\n", rowP[i]);
    }
    for (j = 0; j < C; j++) fprintf(f, "%d ", colP[j]);
    fclose(f);

    printf("\nSender: data sent to channel.txt\n");
    return 0;
}

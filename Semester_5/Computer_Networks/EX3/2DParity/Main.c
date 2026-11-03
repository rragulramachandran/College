#include <stdio.h>
#include "parity.h"

int main()
{
    int data[MAX_ROWS][MAX_COLS];
    int matrix[MAX_ROWS][MAX_COLS];
    int recovered[MAX_ROWS][MAX_COLS];
    int R, C;

    printf("Enter number of data rows: ");
    scanf("%d", &R);
    printf("Enter number of data columns (bits per row): ");
    scanf("%d", &C);

    printf("Enter the %d x %d data bits, row by row:\n", R, C);
    for (int i = 0; i < R; i++)
    {
        printf("Row %d: ", i);
        for (int j = 0; j < C; j++)
        {
            scanf("%d", &data[i][j]);
        }
    }

    generateParityMatrix(data, R, C, matrix);

    int errRow, errCol;
    printf("\nSimulate an error? Enter row and column to flip (0-indexed).\n");
    printf("Valid rows: 0 to %d, valid columns: 0 to %d. Enter -1 -1 for no error.\n",
           R, C);
    printf("Row: ");
    scanf("%d", &errRow);
    printf("Column: ");
    scanf("%d", &errCol);

    if (errRow >= 0 && errRow <= R && errCol >= 0 && errCol <= C)
    {
        matrix[errRow][errCol] ^= 1;
        printf("[Channel] Bit at (%d, %d) flipped during transmission.\n",
               errRow, errCol);
        printMatrix(matrix, R + 1, C + 1, "[Channel] Transmitted (corrupted) matrix:");
    }
    else
    {
        printf("[Channel] No error introduced. Matrix transmitted as-is.\n");
    }

    printf("--- Receiver Processing ---\n");
    int foundRow, foundCol;
    detectAndCorrectError2D(matrix, R, C, &foundRow, &foundCol);
    extractData2D(matrix, R, C, recovered);

    int match = 1;
    for (int i = 0; i < R && match; i++)
    {
        for (int j = 0; j < C; j++)
        {
            if (data[i][j] != recovered[i][j])
            {
                match = 0;
                break;
            }
        }
    }

    printf("Result: %s\n", match ? "SUCCESS - recovered data matches original!"
                                  : "FAILURE - recovered data does not match!");

    return 0;
}

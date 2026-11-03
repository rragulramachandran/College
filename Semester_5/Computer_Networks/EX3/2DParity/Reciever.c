#include <stdio.h>
#include "parity.h"

int detectAndCorrectError2D(int matrix[MAX_ROWS][MAX_COLS], int R, int C,
                             int *errorRow, int *errorCol)
{
    *errorRow = -1;
    *errorCol = -1;

    for (int i = 0; i < R; i++)
    {
        int count = 0;
        for (int j = 0; j < C; j++)
        {
            count += matrix[i][j];
        }
        int expectedParity = (count % 2 == 0) ? 0 : 1;
        if (expectedParity != matrix[i][C])
        {
            *errorRow = i;
        }
    }

    for (int j = 0; j < C; j++)
    {
        int count = 0;
        for (int i = 0; i < R; i++)
        {
            count += matrix[i][j];
        }
        int expectedParity = (count % 2 == 0) ? 0 : 1;
        if (expectedParity != matrix[R][j])
        {
            *errorCol = j;
        }
    }

    if (*errorRow == -1 && *errorCol == -1)
    {
        printf("[Receiver] No error detected.\n");
        return 0;
    }

    if (*errorRow != -1 && *errorCol != -1)
    {
        printf("[Receiver] Error detected at row %d, column %d (0-indexed). "
               "Correcting it...\n", *errorRow, *errorCol);
        matrix[*errorRow][*errorCol] ^= 1;
    }
    else if (*errorRow != -1)
    {
        printf("[Receiver] Error detected in the ROW PARITY bit of row %d. "
               "Correcting it...\n", *errorRow);
        matrix[*errorRow][C] ^= 1;
    }
    else
    {
        printf("[Receiver] Error detected in the COLUMN PARITY bit of column %d. "
               "Correcting it...\n", *errorCol);
        matrix[R][*errorCol] ^= 1;
    }

    printf("[Receiver] Corrected full matrix:\n");
    printMatrix(matrix, R + 1, C + 1, "");

    return 1;
}

void extractData2D(int matrix[MAX_ROWS][MAX_COLS], int R, int C,
                    int data[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            data[i][j] = matrix[i][j];
        }
    }
    printf("[Receiver] Extracted original data:\n");
    printMatrix(data, R, C, "");
}

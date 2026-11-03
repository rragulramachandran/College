#include <stdio.h>
#include "parity.h"

void printMatrix(int matrix[MAX_ROWS][MAX_COLS], int R, int C, const char *label)
{
    printf("%s\n", label);
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            printf("%d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void generateParityMatrix(int data[MAX_ROWS][MAX_COLS], int R, int C,
                           int matrix[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            matrix[i][j] = data[i][j];
        }
    }

    for (int i = 0; i < R; i++)
    {
        int count = 0;
        for (int j = 0; j < C; j++)
        {
            count += data[i][j];
        }
        matrix[i][C] = (count % 2 == 0) ? 0 : 1;
    }

    for (int j = 0; j < C; j++)
    {
        int count = 0;
        for (int i = 0; i < R; i++)
        {
            count += data[i][j];
        }
        matrix[R][j] = (count % 2 == 0) ? 0 : 1;
    }

    int cornerCount = 0;
    for (int j = 0; j < C; j++)
    {
        cornerCount += matrix[R][j];
    }
    matrix[R][C] = (cornerCount % 2 == 0) ? 0 : 1;

    printf("\n[Sender] Original Data (%d rows x %d cols):\n", R, C);
    printMatrix(data, R, C, "");

    printf("[Sender] Full matrix with row parity (last column), column parity\n"
           "(last row) and corner bit (bottom-right):\n");
    printMatrix(matrix, R + 1, C + 1, "");

    printf("[Sender] Data to be sent (each row including its parity bit):\n");
    for (int i = 0; i <= R; i++)
    {
        for (int j = 0; j <= C; j++)
        {
            printf("%d", matrix[i][j]);
        }
        printf(" ");
    }
    printf("\n");
}

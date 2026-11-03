#ifndef PARITY2D_H
#define PARITY2D_H

#define MAX_ROWS 16
#define MAX_COLS 16

void generateParityMatrix(int data[MAX_ROWS][MAX_COLS], int R, int C,
                           int matrix[MAX_ROWS][MAX_COLS]);

int detectAndCorrectError2D(int matrix[MAX_ROWS][MAX_COLS], int R, int C,
                             int *errorRow, int *errorCol);

void extractData2D(int matrix[MAX_ROWS][MAX_COLS], int R, int C,
                    int data[MAX_ROWS][MAX_COLS]);

void printMatrix(int matrix[MAX_ROWS][MAX_COLS], int R, int C, const char *label);

#endif

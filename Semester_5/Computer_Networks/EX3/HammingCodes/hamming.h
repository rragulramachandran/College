#ifndef HAMMING_H
#define HAMMING_H

#define MAX_BITS 64

int isPowerOfTwo(int n);

int calculateRedundantBits(int m);

void generateHammingCode(int data[], int m, int hammingCode[], int *n);

int detectAndCorrectError(int received[], int n);

void extractData(int hammingCode[], int n, int data[], int m);

#endif
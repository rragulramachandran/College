#include "hamming.h"
#include <stdio.h>

int isPowerOfTwo(int n) { return n && !(n & (n - 1)); }

int calculateRedundantBits(int m) {
  int r = 0;
  while ((1 << r) < (m + r + 1)) {
    r++;
  }
  return r;
}

void generateHammingCode(int data[], int m, int hammingCode[], int *n) {
  int r = calculateRedundantBits(m);
  int total = m + r;
  *n = total;

  int j = 0;

  for (int i = 1; i <= total; i++) {
    if (isPowerOfTwo(i)) {
      hammingCode[i] = 0;
    } else {
      hammingCode[i] = data[j++];
    }
  }

  for (int i = 0; i < r; i++) {
    int p = 1 << i;
    int count = 0;

    for (int pos = 1; pos <= total; pos++) {
      if ((pos & p) != 0 && pos != p) {
        count += hammingCode[pos];
      }
    }

    hammingCode[p] = (count % 2 == 0) ? 0 : 1;
  }

  printf("\n[Sender] Data bits (m=%d), Redundant bits (r=%d), Total length "
         "(n=%d)\n",
         m, r, total);
  printf("[Sender] Generated Hamming Code: ");
  for (int i = 1; i <= total; i++) {
    printf("%d", hammingCode[i]);
  }
  printf("\n");
}
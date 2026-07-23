#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void calculate_the_maximum(int n, int k) {
  int max_and = 0;
  int max_or = 0;
  int max_xor = 0;

  for (int i = 1; i <= n; i++) {
    for (int j = i + 1; j <= n; j++) {
      int current_and = i & j;
      int current_or = i | j;
      int current_xor = i ^ j;

      if (current_and < k && current_and > max_and) {
        max_and = current_and;
      }
      if (current_or < k && current_or > max_or) {
        max_or = current_or;
      }
      if (current_xor < k && current_xor > max_xor) {
        max_xor = current_xor;
      }
    }
  }

  printf("%d\n%d\n%d\n", max_and, max_or, max_xor);
}

int main() {
  int n, k;

  if (scanf("%d %d", &n, &k) == 2) {
    calculate_the_maximum(n, k);
  }

  return 0;
}

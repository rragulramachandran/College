#include <stdio.h>
#include <string.h>

char data[10][100];
char checksum[100];
char transmitted[1100];

int n;
int wordSize;

void addBinary(char a[], char b[], char result[]);
void sender();
void receiver();

int main() {
  printf("Enter number of words: ");
  scanf("%d", &n);

  printf("Enter word size: ");
  scanf("%d", &wordSize);

  for (int i = 0; i < n; i++) {
    printf("Enter word %d: ", i + 1);
    scanf("%s", data[i]);
  }

  sender();
  receiver();

  return 0;
}

void addBinary(char a[], char b[], char result[]) {
  int carry = 0;

  for (int i = wordSize - 1; i >= 0; i--) {
    int sum = (a[i] - '0') + (b[i] - '0') + carry;

    result[i] = (sum % 2) + '0';

    carry = sum / 2;
  }

  result[wordSize] = '\0';

  // End-around carry
  if (carry == 1) {
    char one[100];

    for (int i = 0; i < wordSize - 1; i++)
      one[i] = '0';

    one[wordSize - 1] = '1';
    one[wordSize] = '\0';

    char temp[100];

    strcpy(temp, result);

    addBinary(temp, one, result);
  }
}

void sender() 
{
  char sum[100];
  char temp[100];

  // Start with first data word
  strcpy(sum, data[0]);

  // Add remaining words
  for (int i = 1; i < n; i++) 
  {
    addBinary(sum, data[i], temp);
    strcpy(sum, temp);
  }

  // Take 1's complement
  for (int i = 0; i < wordSize; i++) 
  {
    if (sum[i] == '0')
      checksum[i] = '1';
    else
      checksum[i] = '0';
  }

  checksum[wordSize] = '\0';

  printf("\nSum: %s", sum);
  printf("\nChecksum: %s", checksum);

  // Create transmitted data
  transmitted[0] = '\0';

  for (int i = 0; i < n; i++)
    strcat(transmitted, data[i]);

  strcat(transmitted, checksum);

  printf("\nTransmitted Data: %s\n", transmitted);
}

void receiver() 
{
  char sum[100];
  char temp[100];

  // Start with first word
  strncpy(sum, transmitted, wordSize);
  sum[wordSize] = '\0';

  // Add all remaining words including checksum
  for (int i = 1; i <= n; i++) 
  {
    strncpy(temp, transmitted + i * wordSize, wordSize);
    temp[wordSize] = '\0';

    char result[100];

    addBinary(sum, temp, result);
    strcpy(sum, result);
  }

  printf("\nReceiver Sum: %s\n", sum);

  // Check whether result is all 1's
  for (int i = 0; i < wordSize; i++) 
  {
    if (sum[i] != '1') 
    {
      printf("Error detected.\n");
      return;
    }
  }

  printf("No error detected.\n");
}
#include <stdio.h>
#include <string.h>

char data[100];
char generator[50];
char codeword[200];

void sender();
void receiver();

int main() 
{
  char data[100];
  char generator[100];

  printf("Enter the data : ");
  scanf("%s", data);

  printf("Enter the generator polynomial : ");
  scanf("%s", generator);

  sender(data, generator);
  return 0;
}

void sender() 
{
  int dataLen = strlen(data);
  int genLen = strlen(generator);

  char temp[200];

  strcpy(temp, data);

  for (int i = 0; i < genLen - 1; i++) 
  {
    temp[dataLen + i] = '0';
  }

  temp[dataLen + genLen - 1] = '\0';

  for (int i = 0; i < dataLen; i++) 
  {
    if (temp[i] == '1') 
    {
      for (int j = 0; j < genLen; j++) 
      {
        if (temp[i + j] == generator[j])
          temp[i + j] = '0';
        else
          temp[i + j] = '1';
      }
    }
  }

  strcpy(codeword, data);

  for (int i = 0; i < genLen - 1; i++) 
  {
    codeword[dataLen + i] = temp[dataLen + i];
  }

  codeword[dataLen + genLen - 1] = '\0';

  printf("\nCRC: ");

  for (int i = dataLen; i < dataLen + genLen - 1; i++) 
  {
    printf("%c", temp[i]);
  }

  printf("\nTransmitted Frame: %s\n", codeword);
}

void receive() 
{
  int codeLen = strlen(codeword);
  int genLen = strlen(generator);

  char temp[200];

  strcpy(temp, codeword);

  for (int i = 0; i < codeLen - genLen; i++) 
  {
    if (temp[i] == '1') 
    {
      for (int j = 0; j < genLen; j++) 
      {
        if (generator[j] == temp[i + j]) 
        {
          temp[i + j] = '0';
        } 
        else 
        {
          temp[i + j] = '1';
        }
      }
    }
  }

  for (int i = codeLen - genLen; i < codeLen; i++) 
  {
    if (temp[i] == '1') 
    {
      printf("Error");
      return;
    }
  }

  printf("No error");
}
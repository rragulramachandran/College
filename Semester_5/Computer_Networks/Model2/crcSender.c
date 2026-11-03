#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main()
{
    char data[200], gen[200];

    scanf("%s", data);
    scanf("%s", gen);

    int dataLen = strlen(data);
    int genLen = strlen(gen);

    printf("Data              : %s\n", data);
    printf("Data Length       : %d\n", dataLen);
    printf("Generator         : %s\n", gen);
    printf("Generator Length  : %d\n", genLen);

    char temp[200];

    strcpy(temp, data);

    int j = dataLen;
    for(int i = 0; i < genLen - 1; i++)
    {
        temp[i + j] = '0';
    }

    temp[dataLen + genLen] = '\0';

    printf("Temporary         : %s\n", temp);

    for(int i = 0; i < dataLen; i++)
    {
        if(temp[i] == '1')
        {
            for(int j = 0; j < genLen; j++)
            {
                if(temp[i + j] == gen[j])
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

    printf("After Division    : %s\n", temp);

    char code[200];
    strcpy(code, data);
    for(int i = dataLen; i < dataLen + genLen; i++)
    {
        code[i] = temp[i];
    }

    printf("Final Sender Side : %s\n", code);
    return 0;
}
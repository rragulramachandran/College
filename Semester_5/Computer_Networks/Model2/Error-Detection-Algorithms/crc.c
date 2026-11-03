#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main()
{
    char data[100], gen[100];
    printf("Enter Data : ");
    scanf("%s", data);

    printf("Enter Generator : : ");
    scanf("%s", gen);

    int dataLen = strlen(data);
    int genLen = strlen(gen);

    printf("Data Length : %d\n\n", dataLen);
    printf("Generator Length : %d\n\n", genLen);

    char temp[200];
    strcpy(temp, data);

    for(int i = 0; i < genLen - 1; i++)
    {
        temp[i + dataLen] = '0';
    }

    temp[dataLen + genLen - 1] = '\0';

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

    char code[200];
    strcpy(code, data);

    for(int i = 0; i < genLen - 1; i++)
    {
        code[i + dataLen] = temp[i + dataLen];
    }

    code[dataLen + genLen - 1] = '\0';

    printf("Sender Side : %s\n\n", code);

/* -----------------------RECIEVER----------------------*/

    printf("Reciever : Checking...\n\n");

    char result[200];
    strcpy(result, code);

    for(int i = 0; i < dataLen; i++)
    {
        if(result[i] == '1')
        {
            for(int j = 0; j < genLen; j++)
            {
                if(result[i + j] == gen[j])
                {
                    result[i + j] = '0';
                }
                else
                {
                    result[i + j] = '1';
                }
            }
        }
    }

    int error = 0;
    for(int i = dataLen; i < dataLen + genLen - 1; i++)
    {
        if(result[i] != '0')
        {
            error = 1;
            break;
        }
    }

    char * answer = (error) ? "Error : Remainder Not Zero\n\n" : "Succcessful : Remainder all 0\n\n";
    printf("Reciever Side : %s\n\n%s", result, answer);
    return 0;
}
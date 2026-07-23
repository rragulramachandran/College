#include <stdio.h>
#include <string.h>

void stuffing(char body[], char stuffed[])
{
    int i = 0, j = 0;

    while (body[i] != '\0')
    {
        if (strncmp(&body[i], "DLE", 3) == 0)
        {
            strcpy(&stuffed[j], "DLEDLE");
            i += 3;
            j += 6;
        }
        else if (strncmp(&body[i], "STX", 3) == 0)
        {
            strcpy(&stuffed[j], "DLESTX");
            i += 3;
            j += 6;
        }
        else if (strncmp(&body[i], "ETX", 3) == 0)
        {
            strcpy(&stuffed[j], "DLEETX");
            i += 3;
            j += 6;
        }
        else if (strncmp(&body[i], "SOH", 3) == 0)
        {
            strcpy(&stuffed[j], "DLESOH");
            i += 3;
            j += 6;
        }
        else if (strncmp(&body[i], "SYN", 3) == 0)
        {
            strcpy(&stuffed[j], "DLESYN");
            i += 3;
            j += 6;
        }
        else
        {
            stuffed[j++] = body[i++];
        }
    }

    stuffed[j] = '\0';
}

void destuffing(char stuffed[], char body[])
{
    int i = 0, j = 0;

    while (stuffed[i] != '\0')
    {
        if (strncmp(&stuffed[i], "DLEDLE", 6) == 0)
        {
            strcpy(&body[j], "DLE");
            i += 6;
            j += 3;
        }
        else if (strncmp(&stuffed[i], "DLESTX", 6) == 0)
        {
            strcpy(&body[j], "STX");
            i += 6;
            j += 3;
        }
        else if (strncmp(&stuffed[i], "DLEETX", 6) == 0)
        {
            strcpy(&body[j], "ETX");
            i += 6;
            j += 3;
        }
        else if (strncmp(&stuffed[i], "DLESOH", 6) == 0)
        {
            strcpy(&body[j], "SOH");
            i += 6;
            j += 3;
        }
        else if (strncmp(&stuffed[i], "DLESYN", 6) == 0)
        {
            strcpy(&body[j], "SYN");
            i += 6;
            j += 3;
        }
        else
        {
            body[j++] = stuffed[i++];
        }
    }

    body[j] = '\0';
}

void stringToBinary(char* str, char* binaryStr)
{
    int idx = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        for (int j = 7; j >= 0; j--) {
            binaryStr[idx++] = ((str[i] >> j) & 1) ? '1' : '0';
        }
    }
    binaryStr[idx] = '\0';
}

void binaryToString(char* binaryStr, char* str)
{
    int idx = 0;
    for (int i = 0; binaryStr[i] != '\0'; i += 8) {
        char c = 0;
        for (int j = 0; j < 8; j++) {
            c = (c << 1) | (binaryStr[i + j] - '0');
        }
        str[idx++] = c;
    }
    str[idx] = '\0';
}

int main()
{
    char header[50];
    char body[200];
    char stuffedBody[400];
    char binaryBody[3200];
    char receivedBinaryBody[3200];
    char receivedStuffedBody[400];
    char destuffedBody[200];

    printf("========== BISYNC CHARACTER STUFFING ==========\n");

    printf("Enter Header : ");
    scanf("%s", header);

    printf("Enter Body   : ");
    scanf("%s", body);

    stuffing(body, stuffedBody);

    printf("\n------------ SENDER ------------\n");

    printf("Original Frame :\n");
    printf("SYN SYN SOH %s STX %s ETX CRC\n", header, body);

    printf("\nStuffed Frame :\n");
    printf("SYN SYN SOH %s STX %s ETX CRC\n", header, stuffedBody);

    stringToBinary(stuffedBody, binaryBody);
    printf("\nBinary Stuffed Body :\n%s\n", binaryBody);

    FILE *fp = fopen("message.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "%s", binaryBody);
        fclose(fp);
        printf("\n[Binary message stored in message.txt]\n");
    }

    printf("\n----------- RECEIVER -----------\n");

    fp = fopen("message.txt", "r");
    if (fp != NULL) {
        fscanf(fp, "%s", receivedBinaryBody);
        fclose(fp);
        printf("[Binary message read from message.txt]\n");
    } else {
        strcpy(receivedBinaryBody, binaryBody);
    }

    binaryToString(receivedBinaryBody, receivedStuffedBody);

    printf("\nReceived Frame :\n");
    printf("SYN SYN SOH %s STX %s ETX CRC\n", header, receivedStuffedBody);

    destuffing(receivedStuffedBody, destuffedBody);

    printf("\nDestuffed Frame :\n");
    printf("SYN SYN SOH %s STX %s ETX CRC\n", header, destuffedBody);

    return 0;
}
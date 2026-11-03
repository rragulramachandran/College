#include <stdio.h>
#include <string.h>

typedef struct {
    char syn1[4];
    char syn2[4];
    char soh[4];
    char header[50];
    char stx[4];
    char body[400];
    char etx[4];
    char crc[4];
} BisyncFrame;

char header[50];
char body[200];
char stuffedBody[400];
char binaryBody[3200];
char receivedBinaryBody[3200];
char receivedStuffedBody[400];
char destuffedBody[200];

void printFrame(BisyncFrame f) {
    printf("%s %s %s %s %s %s %s %s\n", f.syn1, f.syn2, f.soh, f.header, f.stx, f.body, f.etx, f.crc);
}

void buildFrame(BisyncFrame *f, const char *header, const char *body) {
    strcpy(f->syn1, "SYN");
    strcpy(f->syn2, "SYN");
    strcpy(f->soh, "SOH");
    strcpy(f->header, header);
    strcpy(f->stx, "STX");
    strcpy(f->body, body);
    strcpy(f->etx, "ETX");
    strcpy(f->crc, "CRC");
}

void stuffing(void)
{
    int i = 0, j = 0;

    while (body[i] != '\0')
    {
        if (strncmp(&body[i], "DLE", 3) == 0)
        {
            strcpy(&stuffedBody[j], "DLEDLE");
            i += 3;
            j += 6;
        }
        else if (strncmp(&body[i], "STX", 3) == 0)
        {
            strcpy(&stuffedBody[j], "DLESTX");
            i += 3;
            j += 6;
        }
        else if (strncmp(&body[i], "ETX", 3) == 0)
        {
            strcpy(&stuffedBody[j], "DLEETX");
            i += 3;
            j += 6;
        }
        else if (strncmp(&body[i], "SOH", 3) == 0)
        {
            strcpy(&stuffedBody[j], "DLESOH");
            i += 3;
            j += 6;
        }
        else if (strncmp(&body[i], "SYN", 3) == 0)
        {
            strcpy(&stuffedBody[j], "DLESYN");
            i += 3;
            j += 6;
        }
        else
        {
            stuffedBody[j++] = body[i++];
        }
    }

    stuffedBody[j] = '\0';
}

void destuffing(void)
{
    int i = 0, j = 0;

    while (receivedStuffedBody[i] != '\0')
    {
        if (strncmp(&receivedStuffedBody[i], "DLEDLE", 6) == 0)
        {
            strcpy(&destuffedBody[j], "DLE");
            i += 6;
            j += 3;
        }
        else if (strncmp(&receivedStuffedBody[i], "DLESTX", 6) == 0)
        {
            strcpy(&destuffedBody[j], "STX");
            i += 6;
            j += 3;
        }
        else if (strncmp(&receivedStuffedBody[i], "DLEETX", 6) == 0)
        {
            strcpy(&destuffedBody[j], "ETX");
            i += 6;
            j += 3;
        }
        else if (strncmp(&receivedStuffedBody[i], "DLESOH", 6) == 0)
        {
            strcpy(&destuffedBody[j], "SOH");
            i += 6;
            j += 3;
        }
        else if (strncmp(&receivedStuffedBody[i], "DLESYN", 6) == 0)
        {
            strcpy(&destuffedBody[j], "SYN");
            i += 6;
            j += 3;
        }
        else
        {
            destuffedBody[j++] = receivedStuffedBody[i++];
        }
    }

    destuffedBody[j] = '\0';
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
    printf("========== BISYNC CHARACTER STUFFING ==========\n");

    printf("Enter Header : ");
    scanf("%s", header);

    FILE *inFile = fopen("input.txt", "r");
    if (inFile != NULL) {
        int len = 0;
        int ch;
        while ((ch = fgetc(inFile)) != EOF && len < 199) {
            if (ch == '\n' || ch == '\r') continue;
            body[len++] = (char)ch;
        }
        body[len] = '\0';
        fclose(inFile);
        printf("Body read from input.txt : %s\n", body);
    } else {
        printf("Error: Cannot open input.txt\n");
        return 1;
    }

    stuffing();

    printf("\n------------ SENDER ------------\n");

    BisyncFrame originalFrame;
    buildFrame(&originalFrame, header, body);
    printf("Original Frame :\n");
    printFrame(originalFrame);

    BisyncFrame stuffedFrame;
    buildFrame(&stuffedFrame, header, stuffedBody);
    printf("\nStuffed Frame :\n");
    printFrame(stuffedFrame);

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

    BisyncFrame receivedFrame;
    buildFrame(&receivedFrame, header, receivedStuffedBody);
    printf("\nReceived Frame :\n");
    printFrame(receivedFrame);

    destuffing();

    BisyncFrame destuffedFrame;
    buildFrame(&destuffedFrame, header, destuffedBody);
    printf("\nDestuffed Frame :\n");
    printFrame(destuffedFrame);

    FILE *outFile = fopen("output.txt", "w");
    if (outFile != NULL) {
        fprintf(outFile, "%s %s %s %s %s %s %s %s\n", 
                destuffedFrame.syn1, destuffedFrame.syn2, destuffedFrame.soh, 
                destuffedFrame.header, destuffedFrame.stx, destuffedFrame.body, 
                destuffedFrame.etx, destuffedFrame.crc);
        fclose(outFile);
        printf("\n[Final format stored in output.txt]\n");
    } else {
        printf("\nError: Cannot open output.txt\n");
    }

    return 0;
}
#include <stdio.h>
#include <string.h>

typedef struct {
    char flag1[9];
    char header[400];
    char body[3200];
    char crc[17];
    char flag2[9];
} HDLCFrame;

char header[50];
char binaryHeader[400];
char body[200];
char binaryBody[1600];
char stuffedBody[3200];
char receivedStuffedBody[3200];
char destuffedBody[1600];
char receivedBody[200];

void printFrame(HDLCFrame f) {
    printf("%s %s %s %s %s\n", f.flag1, f.header, f.body, f.crc, f.flag2);
}

void buildFrame(HDLCFrame *f, const char *hdr, const char *payload) {
    strcpy(f->flag1, "01111110");
    strcpy(f->header, hdr);
    strcpy(f->body, payload);
    strcpy(f->crc, "1100110011001100"); // Dummy 16-bit CRC
    strcpy(f->flag2, "01111110");
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

void stuffing(void)
{
    int i = 0, j = 0;
    int consecutive_ones = 0;

    // Bit stuffing: Insert '0' after every 5 consecutive '1's
    while (binaryBody[i] != '\0')
    {
        if (binaryBody[i] == '1')
        {
            consecutive_ones++;
            stuffedBody[j++] = binaryBody[i++];
            if (consecutive_ones == 5)
            {
                stuffedBody[j++] = '0'; // stuff a 0
                consecutive_ones = 0;
            }
        }
        else
        {
            consecutive_ones = 0;
            stuffedBody[j++] = binaryBody[i++];
        }
    }
    stuffedBody[j] = '\0';
}

void destuffing(void)
{
    int i = 0, j = 0;
    int consecutive_ones = 0;

    // Bit destuffing: Discard '0' that follows 5 consecutive '1's
    while (receivedStuffedBody[i] != '\0')
    {
        if (receivedStuffedBody[i] == '1')
        {
            consecutive_ones++;
            destuffedBody[j++] = receivedStuffedBody[i++];
            if (consecutive_ones == 5)
            {
                if (receivedStuffedBody[i] == '0')
                {
                    // It's a stuffed 0, discard it
                    i++;
                    consecutive_ones = 0;
                }
                else if (receivedStuffedBody[i] == '1')
                {
                    // 6 consecutive 1s: Look at 7th bit for Flag (0) or Error (1)
                    // In this simulation, we just reset the counter.
                    consecutive_ones = 0;
                }
            }
        }
        else
        {
            consecutive_ones = 0;
            destuffedBody[j++] = receivedStuffedBody[i++];
        }
    }
    destuffedBody[j] = '\0';
}

int main()
{
    printf("========== HIGH LEVEL DATA LINK CONTROL (HDLC) ==========\n");

    printf("Enter Header (ASCII) : ");
    scanf("%s", header);
    stringToBinary(header, binaryHeader);

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

    // HDLC stuffing happens at the binary level
    stringToBinary(body, binaryBody);

    stuffing();

    printf("\n------------ SENDER ------------\n");

    HDLCFrame originalFrame;
    buildFrame(&originalFrame, binaryHeader, binaryBody);
    printf("Original Frame :\n");
    printFrame(originalFrame);

    HDLCFrame stuffedFrame;
    buildFrame(&stuffedFrame, binaryHeader, stuffedBody);
    printf("\nStuffed Frame :\n");
    printFrame(stuffedFrame);

    FILE *fp = fopen("message_hdlc.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "%s", stuffedBody);
        fclose(fp);
        printf("\n[Binary message stored in message_hdlc.txt]\n");
    }

    printf("\n----------- RECEIVER -----------\n");

    fp = fopen("message_hdlc.txt", "r");
    if (fp != NULL) {
        fscanf(fp, "%s", receivedStuffedBody);
        fclose(fp);
        printf("[Binary message read from message_hdlc.txt]\n");
    } else {
        strcpy(receivedStuffedBody, stuffedBody);
    }

    HDLCFrame receivedFrame;
    buildFrame(&receivedFrame, binaryHeader, receivedStuffedBody);
    printf("\nReceived Frame :\n");
    printFrame(receivedFrame);

    destuffing();

    HDLCFrame destuffedFrame;
    buildFrame(&destuffedFrame, binaryHeader, destuffedBody);
    printf("\nDestuffed Frame :\n");
    printFrame(destuffedFrame);

    binaryToString(destuffedBody, receivedBody);
    printf("\nDecoded Body: %s\n", receivedBody);

    FILE *outFile = fopen("output_hdlc.txt", "w");
    if (outFile != NULL) {
        fprintf(outFile, "%s %s %s %s %s\n", 
                destuffedFrame.flag1, destuffedFrame.header, destuffedFrame.body, 
                destuffedFrame.crc, destuffedFrame.flag2);
        fclose(outFile);
        printf("\n[Final format stored in output_hdlc.txt]\n");
    } else {
        printf("\nError: Cannot open output_hdlc.txt\n");
    }

    return 0;
}

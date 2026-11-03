#include <stdio.h>
#include <string.h>

typedef struct {
    char flag1[9];
    char address[10];
    char ctrl[9];
    char body[3200];
    char fcs[4];
    char flag2[9];
} PPPFrame;

char body[200];
char binaryBody[1600];
char stuffedBody[3200];
char receivedStuffedBody[3200];
char destuffedBody[1600];
char receivedBody[200];

void printFrame(PPPFrame f) {
    printf("%s %s %s %s %s %s\n", f.flag1, f.address, f.ctrl, f.body, f.fcs, f.flag2);
}

void buildFrame(PPPFrame *f, const char *payload) {
    strcpy(f->flag1, "01111110");
    // Standard PPP address is 8 bits (0xFF), so we use 8 ones.
    strcpy(f->address, "11111111"); 
    strcpy(f->ctrl, "00000011");
    strcpy(f->body, payload);
    strcpy(f->fcs, "FCS");
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

    // We process byte by byte (8 bits at a time)
    while (binaryBody[i] != '\0')
    {
        if (strncmp(&binaryBody[i], "01111110", 8) == 0) // flag
        {
            strcpy(&stuffedBody[j], "01111101"); // esc
            strcpy(&stuffedBody[j+8], "01111110"); // flag
            i += 8;
            j += 16;
        }
        else if (strncmp(&binaryBody[i], "01111101", 8) == 0) // esc
        {
            strcpy(&stuffedBody[j], "01111101"); // esc
            strcpy(&stuffedBody[j+8], "01111101"); // esc
            i += 8;
            j += 16;
        }
        else
        {
            for (int k = 0; k < 8 && binaryBody[i] != '\0'; k++)
            {
                stuffedBody[j++] = binaryBody[i++];
            }
        }
    }

    stuffedBody[j] = '\0';
}

void destuffing(void)
{
    int i = 0, j = 0;

    while (receivedStuffedBody[i] != '\0')
    {
        if (strncmp(&receivedStuffedBody[i], "01111101", 8) == 0) // esc
        {
            if (strncmp(&receivedStuffedBody[i+8], "01111110", 8) == 0)
            {
                strcpy(&destuffedBody[j], "01111110");
                i += 16;
                j += 8;
            }
            else if (strncmp(&receivedStuffedBody[i+8], "01111101", 8) == 0)
            {
                strcpy(&destuffedBody[j], "01111101");
                i += 16;
                j += 8;
            }
            else
            {
                // Unescaped ESC or invalid frame
                for (int k = 0; k < 8 && receivedStuffedBody[i] != '\0'; k++)
                {
                    destuffedBody[j++] = receivedStuffedBody[i++];
                }
            }
        }
        else
        {
            for (int k = 0; k < 8 && receivedStuffedBody[i] != '\0'; k++)
            {
                destuffedBody[j++] = receivedStuffedBody[i++];
            }
        }
    }

    destuffedBody[j] = '\0';
}

int main()
{
    printf("========== POINT-TO-POINT PROTOCOL (PPP) ==========\n");

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

    // PPP stuffing happens at the binary level
    stringToBinary(body, binaryBody);

    stuffing();

    printf("\n------------ SENDER ------------\n");

    PPPFrame originalFrame;
    buildFrame(&originalFrame, binaryBody);
    printf("Original Frame :\n");
    printFrame(originalFrame);

    PPPFrame stuffedFrame;
    buildFrame(&stuffedFrame, stuffedBody);
    printf("\nStuffed Frame :\n");
    printFrame(stuffedFrame);

    FILE *fp = fopen("message_ppp.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "%s", stuffedBody);
        fclose(fp);
        printf("\n[Binary message stored in message_ppp.txt]\n");
    }

    printf("\n----------- RECEIVER -----------\n");

    fp = fopen("message_ppp.txt", "r");
    if (fp != NULL) {
        fscanf(fp, "%s", receivedStuffedBody);
        fclose(fp);
        printf("[Binary message read from message_ppp.txt]\n");
    } else {
        strcpy(receivedStuffedBody, stuffedBody);
    }

    PPPFrame receivedFrame;
    buildFrame(&receivedFrame, receivedStuffedBody);
    printf("\nReceived Frame :\n");
    printFrame(receivedFrame);

    destuffing();

    PPPFrame destuffedFrame;
    buildFrame(&destuffedFrame, destuffedBody);
    printf("\nDestuffed Frame :\n");
    printFrame(destuffedFrame);

    binaryToString(destuffedBody, receivedBody);
    printf("\nDecoded Body: %s\n", receivedBody);

    FILE *outFile = fopen("output_ppp.txt", "w");
    if (outFile != NULL) {
        fprintf(outFile, "%s %s %s %s %s %s\n", 
                destuffedFrame.flag1, destuffedFrame.address, destuffedFrame.ctrl, 
                destuffedFrame.body, destuffedFrame.fcs, destuffedFrame.flag2);
        fclose(outFile);
        printf("\n[Final format stored in output_ppp.txt]\n");
    } else {
        printf("\nError: Cannot open output_ppp.txt\n");
    }

    return 0;
}

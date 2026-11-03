#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char syn1[4];
    char syn2[4];
    char classField[6];
    int count;
    char header[50];
    char body[400];
    char crc[4];
} DDCMPFrame;

char header[50];
char body[200];
char binaryBody[1600];
char receivedBinaryBody[1600];
char receivedBody[200];
int byteCount;

void printFrame(DDCMPFrame f) {
    printf("%s %s %s %d %s %s %s\n", f.syn1, f.syn2, f.classField, f.count, f.header, f.body, f.crc);
}

void buildFrame(DDCMPFrame *f, const char *hdr, int cnt, const char *payload) {
    strcpy(f->syn1, "SYN");
    strcpy(f->syn2, "SYN");
    strcpy(f->classField, "CLASS");
    f->count = cnt;
    strcpy(f->header, hdr);
    strcpy(f->body, payload);
    strcpy(f->crc, "CRC");
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

// In DDCMP, the receiver relies entirely on the byte count
void binaryToString(char* binaryStr, char* str, int count)
{
    int idx = 0;
    // We strictly decode only 'count' bytes
    for (int i = 0; i < count * 8 && binaryStr[i] != '\0'; i += 8) {
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
    printf("========== DDCMP (BYTE COUNT APPROACH) ==========\n");

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

    // DDCMP relies on the count field (number of bytes in the payload)
    byteCount = strlen(body);

    stringToBinary(body, binaryBody);

    printf("\n------------ SENDER ------------\n");

    DDCMPFrame originalFrame;
    buildFrame(&originalFrame, header, byteCount, body);
    printf("Original Frame :\n");
    printFrame(originalFrame);

    FILE *fp = fopen("message_ddcmp.txt", "w");
    if (fp != NULL) {
        // We write the count first to simulate transmitting the frame header, 
        // followed by the binary body payload.
        fprintf(fp, "%d\n%s", byteCount, binaryBody);
        fclose(fp);
        printf("\n[Binary message stored in message_ddcmp.txt]\n");
    }

    printf("\n----------- RECEIVER -----------\n");

    int receivedCount = 0;
    fp = fopen("message_ddcmp.txt", "r");
    if (fp != NULL) {
        fscanf(fp, "%d", &receivedCount);
        fscanf(fp, "%s", receivedBinaryBody);
        fclose(fp);
        printf("[Binary message read from message_ddcmp.txt]\n");
    } else {
        receivedCount = byteCount;
        strcpy(receivedBinaryBody, binaryBody);
    }

    // The key of DDCMP is reading exactly 'count' bytes from the binary stream
    binaryToString(receivedBinaryBody, receivedBody, receivedCount);

    DDCMPFrame receivedFrame;
    buildFrame(&receivedFrame, header, receivedCount, receivedBody);
    printf("\nReceived Frame :\n");
    printFrame(receivedFrame);

    FILE *outFile = fopen("output_ddcmp.txt", "w");
    if (outFile != NULL) {
        fprintf(outFile, "%s %s %s %d %s %s %s\n", 
                receivedFrame.syn1, receivedFrame.syn2, receivedFrame.classField, 
                receivedFrame.count, receivedFrame.header, receivedFrame.body, receivedFrame.crc);
        fclose(outFile);
        printf("\n[Final format stored in output_ddcmp.txt]\n");
    } else {
        printf("\nError: Cannot open output_ddcmp.txt\n");
    }

    return 0;
}

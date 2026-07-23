#include <stdio.h>
#include <string.h>

#include "Sender.h"

// ---------- CRC IMPLEMENTATION ----------

bool is_binary(const char *s) {
    if (!s) return false;
    for (int i = 0; s[i]; i++) {
        if (s[i] != '0' && s[i] != '1') return false;
    }
    return true;
}

void findXor(const char a[], const char b[], char result[], int n) {
    for (int i = 1; i < n; i++) {
        result[i - 1] = (a[i] == b[i]) ? '0' : '1';
    }
    result[n - 1] = '\0';
}

void mod2div(const char dividend[], const char divisor[], char remainder[]) 
{
    int pick = strlen(divisor);
    int n = strlen(dividend);
    int divisorLen = pick;

    if (divisorLen > n || divisorLen <= 1) 
    {
        remainder[0] = '\0';
        return;
    }

    char tmp[CRC_MAX_LEN];
    char xorResult[CRC_MAX_LEN];
    char zeros[CRC_MAX_LEN];

    for (int i = 0; i < divisorLen; i++) zeros[i] = '0';
    zeros[divisorLen] = '\0';

    strncpy(tmp, dividend, divisorLen);
    tmp[divisorLen] = '\0';

    while (pick < n) 
    {
        if (tmp[0] == '1')
            findXor(divisor, tmp, xorResult, divisorLen);
        else
            findXor(zeros, tmp, xorResult, divisorLen);

        int len = strlen(xorResult);
        if (len + 1 >= CRC_MAX_LEN) break;
        xorResult[len] = dividend[pick];
        xorResult[len + 1] = '\0';

        strncpy(tmp, xorResult, CRC_MAX_LEN - 1);
        tmp[CRC_MAX_LEN - 1] = '\0';
        pick++;
    }

    // Final XOR
    if (tmp[0] == '1')
        findXor(divisor, tmp, remainder, divisorLen);
    else
        findXor(zeros, tmp, remainder, divisorLen);
}

// Encode data with CRC: code = data + remainder
void encodeData(const char data[], const char key[], char code[]) 
{
    if (!is_binary(data) || !is_binary(key) || strlen(key) < 2) 
    {
        printf("Error: Invalid input for CRC encoding\n");
        code[0] = '\0';
        return;
    }

    char paddedData[CRC_MAX_LEN] = {0};
    char remainder[CRC_MAX_LEN] = {0};

    strncpy(paddedData, data, CRC_MAX_LEN - 1);
    int zeros = strlen(key) - 1;
    if (strlen(paddedData) + zeros >= CRC_MAX_LEN) {
        printf("Error: Data too long for CRC buffer\n");
        code[0] = '\0';
        return;
    }
    for (int i = 0; i < zeros; i++) strcat(paddedData, "0");

    mod2div(paddedData, key, remainder);

    strncpy(code, data, CRC_MAX_LEN - 1);
    strncat(code, remainder, CRC_MAX_LEN - strlen(code) - 1);
}

// ---------- SENDER FUNCTIONS ----------

// Build frame with Byte-Count + CRC
void buildFrame(ByteCountFrame *frame, const char *payload, const char *crcKey) {
    frame->frameDelimiter = 0x7E;
    frame->destAddr = 0x01;
    frame->srcAddr = 0x02;

    int dataBytes = strlen(payload) / 8;
    frame->byteCount = (unsigned short)dataBytes;

    strncpy(frame->data, payload, MAX_FRAME_SIZE - 1);
    frame->data[MAX_FRAME_SIZE - 1] = '\0';

    // Compute CRC over: byteCount (2 bytes) + data
    char crcInput[CRC_MAX_LEN] = {0};
    char byteCountBits[17];
    toBinary(frame->byteCount, 16, byteCountBits);
    strcat(crcInput, byteCountBits);
    strcat(crcInput, payload);

    encodeData(crcInput, crcKey, frame->crc);
    // Extract just the remainder (last keyLen-1 bits)
    int remLen = strlen(crcKey) - 1;
    int codeLen = strlen(frame->crc);
    if (codeLen >= remLen) {
        char *remStart = frame->crc + codeLen - remLen;
        memmove(frame->crc, remStart, remLen + 1);
    }

    frame->valid = true;
}

// Send frame (display)
void sendFrame(const ByteCountFrame *frame, const char *crcKey) {
    printf("\n========== SENDER SIDE (Byte-Count Frame) ==========\n");
    printf("Flag (0x7E)     : 0x%02X\n", frame->frameDelimiter);
    printf("Dest Address    : 0x%02X\n", frame->destAddr);
    printf("Src Address     : 0x%02X\n", frame->srcAddr);
    printf("Byte Count      : %u bytes (0x%04X)\n", frame->byteCount, frame->byteCount);
    printf("Data (bits)     : %s\n", frame->data);
    printf("CRC Key         : %s\n", crcKey);
    printf("CRC Remainder   : %s\n", frame->crc);
    printf("Codeword Sent   : %s%s\n", frame->data, frame->crc);
    printf("====================================================\n");
}

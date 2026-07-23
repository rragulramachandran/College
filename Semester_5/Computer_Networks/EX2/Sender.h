#ifndef SENDER_H
#define SENDER_H

#include <stdbool.h>

#define MAX_FRAME_SIZE 1024
#define CRC_MAX_LEN 1024

// Byte-Count Frame structure
typedef struct {
    unsigned char frameDelimiter;   // 0x7E (flag)
    unsigned char destAddr;         // 1 byte dest
    unsigned char srcAddr;          // 1 byte src
    unsigned short byteCount;       // 2 bytes: count of data bytes
    char data[MAX_FRAME_SIZE];      // Data payload (binary string)
    char crc[CRC_MAX_LEN];          // CRC remainder
    bool valid;                     // For receiver validation
} ByteCountFrame;

// CRC helper functions
bool is_binary(const char *s);
void findXor(const char a[], const char b[], char result[], int n);
void mod2div(const char dividend[], const char divisor[], char remainder[]);
void encodeData(const char data[], const char key[], char code[]);

// Sender functions
void buildFrame(ByteCountFrame *frame, const char *payload, const char *crcKey);
void sendFrame(const ByteCountFrame *frame, const char *crcKey);

// toBinary declared here since Sender.c uses it via buildFrame
void toBinary(unsigned long val, int bits, char out[]);

#endif

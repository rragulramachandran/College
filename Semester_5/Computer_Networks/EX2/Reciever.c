#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sender.h"

// Verify received codeword: returns true if no error (remainder all zeros)
bool verifyCRC(const char code[], const char key[]) 
{
    if (!is_binary(code) || !is_binary(key) || strlen(key) < 2) return false;

    char remainder[CRC_MAX_LEN];
    mod2div(code, key, remainder);

    for (int i = 0; remainder[i]; i++) {
        if (remainder[i] == '1') return false;  // Error detected
    }
    return true;  // No error
}

// Receive frame with error injection option
bool receiveFrame(ByteCountFrame *frame, const char *crcKey, bool injectError) {
    printf("\n========== RECEIVER SIDE (Byte-Count Frame) ==========\n");

    // Check flag
    if (frame->frameDelimiter != 0x7E) {
        printf("[Error] Invalid frame delimiter!\n");
        return false;
    }
    printf("[Receiver] Flag (0x7E) verified.\n");

    // Reconstruct codeword for verification: byteCount + data + CRC remainder
    char byteCountBits[17];
    toBinary(frame->byteCount, 16, byteCountBits);
    char codeword[CRC_MAX_LEN];
    snprintf(codeword, CRC_MAX_LEN, "%s%s%s", byteCountBits, frame->data, frame->crc);

    // Inject error if requested
    if (injectError && strlen(codeword) > 0) {
        int pos = rand() % strlen(codeword);
        codeword[pos] = (codeword[pos] == '0') ? '1' : '0';
        printf("\n*** ERROR INJECTED: Flipped bit at position %d ***\n", pos);
        printf("Corrupted Codeword: %s\n", codeword);
    }

    // Verify CRC
    bool crcOk = verifyCRC(codeword, crcKey);

    printf("Byte Count      : %u bytes\n", frame->byteCount);
    printf("Received Data   : %s\n", frame->data);
    printf("CRC Key         : %s\n", crcKey);
    printf("CRC Remainder   : %s\n", frame->crc);
    printf("Codeword Check  : %s\n", codeword);

    if (crcOk) {
        printf("[SUCCESS] CRC verification PASSED - No errors detected!\n");
    } else {
        printf("[ERROR] CRC verification FAILED - Error detected!\n");
    }
    printf("========================================================\n");

    return crcOk;
}

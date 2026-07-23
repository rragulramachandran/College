#ifndef RECIEVER_H
#define RECIEVER_H

#include "Sender.h"

// Verify received codeword: returns true if no error (remainder all zeros)
bool verifyCRC(const char code[], const char key[]);

// Receive frame with error injection option and CRC validation
bool receiveFrame(ByteCountFrame *frame, const char *crcKey, bool injectError);

#endif

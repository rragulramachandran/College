#ifndef CRC_H
#define CRC_H

#define MAX_LEN 256

/* Performs modulo-2 (XOR) binary division, exactly like the long division
   shown in Figure 3.13 / 3.14 (multiply = AND, subtract = XOR, and whenever
   the leftmost bit of the working chunk is 0 we effectively XOR with an
   all-zero divisor).
   dividend : the bit string being divided (data padded with zeros, or a
              received codeword)
   divisor  : the generator polynomial bit string, e.g. "1011" for x^3+x+1
   remainder: output buffer, must be at least (strlen(divisor)) chars,
              receives the final remainder (length = strlen(divisor) - 1) */
void modulo2Division(const char *dividend, const char *divisor, char *remainder);

/* SENDER SIDE
   data      : original dataword bit string, e.g. "1001"
   generator : generator polynomial bit string, e.g. "1011"
   codeword  : output buffer, receives data + CRC remainder appended
               (length = strlen(data) + strlen(generator) - 1) */
void generateCRC(const char *data, const char *generator, char *codeword);

/* RECEIVER SIDE
   codeword  : received codeword bit string (possibly corrupted)
   generator : generator polynomial bit string used by the sender
   syndrome  : output buffer, receives the remainder bits (all zero if the
               data arrived without any detectable error)
   Returns 1 if the codeword is accepted (syndrome is all zero), 0 if it
   should be discarded (a corrupted, nonzero syndrome). */
int checkCRC(const char *codeword, const char *generator, char *syndrome);

#endif
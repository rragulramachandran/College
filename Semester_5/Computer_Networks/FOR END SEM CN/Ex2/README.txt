CHECKSUM AND CRC - SIMPLE AND SENDER/RECEIVER C PROGRAMS

Simple concept programs:
1. simple_checksum.c - calculates an 8-bit checksum for input text.
2. simple_crc.c - calculates the CRC remainder for binary data and a generator.

Sender/receiver programs:
3. checksum_sender.c
4. checksum_receiver.c
5. crc_sender.c
6. crc_receiver.c

Compile each file separately with GCC, for example:
gcc simple_checksum.c -o simple_checksum
gcc simple_crc.c -o simple_crc
gcc checksum_sender.c -o checksum_sender
gcc checksum_receiver.c -o checksum_receiver
gcc crc_sender.c -o crc_sender
gcc crc_receiver.c -o crc_receiver

Checksum notes:
- This is a simplified 8-bit teaching example over text bytes, not the 16-bit
  Internet checksum.
- The sender displays the checksum in hexadecimal; the receiver currently asks
  for its decimal value (convert before entering).

CRC notes:
- Enter binary data and a generator bit string, e.g. 10011.
- The simple CRC program only calculates the remainder.
- The sender/receiver versions create and check a codeword.
- A zero CRC remainder means no error was detected; it does not guarantee that
  no error occurred.

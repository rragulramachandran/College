Stuffing Programs (C)

Files:
1. simple_bit_stuffing.c       - bit stuffing and destuffing in one program
2. simple_byte_stuffing.c      - byte stuffing and destuffing in one program
3. bit_stuffing_sender.c       - sender side bit stuffing
4. bit_stuffing_receiver.c     - receiver side bit destuffing
5. byte_stuffing_sender.c      - sender side byte stuffing
6. byte_stuffing_receiver.c    - receiver side byte destuffing

Compile with GCC, for example:
gcc simple_bit_stuffing.c -o simple_bit_stuffing
gcc simple_byte_stuffing.c -o simple_byte_stuffing
gcc bit_stuffing_sender.c -o bit_stuffing_sender
gcc bit_stuffing_receiver.c -o bit_stuffing_receiver
gcc byte_stuffing_sender.c -o byte_stuffing_sender
gcc byte_stuffing_receiver.c -o byte_stuffing_receiver

Run each executable separately. For sender/receiver programs, copy the sender's
stuffed output and enter it as the receiver's input.

Notes:
- Bit stuffing inserts a 0 after five consecutive 1 bits.
- The simple byte-stuffing example uses '~' as the frame flag and '}' as the
  escape character. These are illustrative characters for this program, not
  a complete PPP implementation.

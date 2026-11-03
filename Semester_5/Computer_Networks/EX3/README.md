# EX3: Error Detection & Correction (2D Parity & Hamming Code)

Implementation of 2D Parity Check and Hamming Code error detection and correction in C.

## Overview

1. **2D Parity Check:**
   - Arranges data bits into a two-dimensional grid ($m \times n$).
   - Calculates horizontal parity bits for each row and vertical parity bits for each column.
   - Detects all 1-bit, 2-bit, and 3-bit errors, and identifies the exact location of single-bit errors.

2. **Hamming Code (7, 4):**
   - Encodes 4 data bits into a 7-bit codeword by inserting 3 redundant parity bits at power-of-2 positions ($p_1, p_2, p_4$).
   - Evaluates syndrome bits ($s_1, s_2, s_3$) to locate and automatically correct single-bit transmission errors.

## Files

| File | Description |
|------|-------------|
| `sender.c(2d parity)` | Sender program for 2D parity encoding and optional error injection |
| `receiver.c(2d parity)` | Receiver verifying row and column parity matrices |
| `sender.c(hamming code)` | Sender calculating Hamming parity bits ($p_1, p_2, p_4$) |
| `receiver.c(hamming code)` | Receiver evaluating syndrome word to detect and flip erroneous bit |
| `Sender.c` | Standalone unified test sender |

## Compilation

```bash
# 2D Parity Check
gcc "sender.c(2d parity)" -o parity_sender
gcc "receiver.c(2d parity)" -o parity_receiver

# Hamming Code
gcc "sender.c(hamming code)" -o hamming_sender
gcc "receiver.c(hamming code)" -o hamming_receiver
```

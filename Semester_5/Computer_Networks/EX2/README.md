# EX2: CRC (Cyclic Redundancy Check) Error Detection

Modular implementation of CRC error detection and verification in C.

## Overview

CRC is a polynomial division-based error detection code operating in modulo-2 arithmetic (XOR). The sender divides the data bit sequence appended with zeros by a generator polynomial, appending the remainder (checksum) to the frame. The receiver divides the received frame by the same generator polynomial; a non-zero remainder indicates transmission corruption.

## Modular Architecture

```text
EX2/
├── header.h                — Common data structures, frame definition, and function prototypes
├── Sender.h / Sender.c     — Sender logic: read payload, compute CRC, optionally inject errors
├── Reciever.h / Reciever.c — Receiver logic: modulo-2 polynomial division, error verification
├── Main.c                  — Entry point orchestrating sender transmission and receiver verification
├── practice_crc.c          — Self-contained standalone CRC implementation
└── practice_checksum.c     — Internet Checksum (ones' complement sum) comparison
```

## Supported Generator Polynomials

- **CRC-12:** $x^{12} + x^{11} + x^3 + x^2 + x + 1$
- **CRC-16:** $x^{16} + x^{15} + x^2 + 1$
- **CRC-CCITT:** $x^{16} + x^{12} + x^5 + 1$

## Compilation

```bash
# Modular build
gcc Main.c Sender.c Reciever.c -o crc_app
./crc_app

# Standalone practice build
gcc practice_crc.c -o practice_crc
./practice_crc
```

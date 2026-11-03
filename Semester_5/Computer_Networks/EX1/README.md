# EX1: Framing Protocols & Character/Bit Stuffing

Implementation of fundamental Data Link layer framing protocols in C.

## Overview

Framing divides raw bit streams into discrete frames and ensures receivers can accurately detect frame boundaries even when control characters appear inside the payload.

## Implemented Protocols

| Protocol | File | Framing Technique | Description |
|----------|------|-------------------|-------------|
| **BISYNC** | `BISYNC.c` | Character Stuffing | Uses `DLE` (Data Link Escape), `STX`, `ETX`. Internal `DLE` characters are escaped by doubling them (`DLE DLE`). |
| **DDCMP** | `DDCMP.c` | Byte-Count Framing | Includes a field specifying the length of the data payload. |
| **HDLC** | `HDLC.c` | Bit Stuffing | Uses delimiter flag `01111110`. Inserts a `0` after any sequence of five consecutive `1`s. Receiver removes the stuffed `0`. |
| **PPP** | `PPP.c` | Byte Stuffing | Uses flag `0x7E` and escape byte `0x7D` (`0x7D 0x5E` for `0x7E`, `0x7D 0x5D` for `0x7D`). |
| **Full Stack** | `LayeringandFraming.c` | Integrated Framing | End-to-end layering with DNS resolution and framing. |

## Compilation

```bash
# Compile individual protocol implementations
gcc BISYNC.c -o bisync
./bisync

gcc HDLC.c -o hdlc
./hdlc

gcc PPP.c -o ppp
./ppp

gcc DDCMP.c -o ddcmp
./ddcmp
```

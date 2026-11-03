# EX0: Layering and Framing Simulation (DDCMP)

Implementation of network layering and byte-count data framing in C.

## Overview

This exercise simulates the journey of an application-layer message traversing the protocol stack:

1. **Application Layer:** Takes user message input and specifies target destination URLs.
2. **Transport Layer:** Port multiplexing and process identification.
3. **Network Layer:** Open-addressing DNS hash table mapping hostnames/URLs to IP addresses, with next-hop IP resolution.
4. **Data Link Layer:** MAC address resolution and **DDCMP** (Digital Data Communications Message Protocol) byte-count framing.

## Files

| File | Description |
|------|-------------|
| `LayeringandFraming.c` | Core simulation: DNS hash table CRUD, protocol headers, and DDCMP byte-count framing |
| `message.txt` | Sample input payload text |
| `test_expect.c` / `test_pty.py` | Automated testing harnesses |
| `generate_record.sh` | Shell script for generating lab record outputs |

## DDCMP Framing Format

DDCMP uses an explicit byte count in its header rather than character stuffing or bit flags:

```text
+-------+-------+---------------+----------+----------+---------+
|  SYN  |  SYN  |  Byte Count   | Sequence | Payload  |   CRC   |
| 1 Byte| 1 Byte|    14 Bits    |  8 Bits  |  N Bytes | 2 Bytes |
+-------+-------+---------------+----------+----------+---------+
```

## Compilation and Execution

```bash
gcc LayeringandFraming.c -o layering
./layering
```

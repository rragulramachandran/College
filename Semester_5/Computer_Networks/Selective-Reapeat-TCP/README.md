# Selective Repeat Sliding Window Protocol

Simulation of Selective Repeat ARQ flow control in C over TCP sockets.

## Overview

Unlike Go-Back-N, **Selective Repeat** resends *only* the specific frames that were lost or corrupted:
- **Sender Window & Receiver Window:** Both windows are of size $W \le 2^{m-1}$.
- **Individual ACKs / NAKs:** The receiver buffers out-of-order frames that fall inside the receiver window and sends selective acknowledgments.
- **Independent Timers:** Each transmitted frame has an independent timer. Only the timed-out frame is retransmitted, maximizing bandwidth utilization.

## Files

| File | Description |
|------|-------------|
| `sender.c` | Selective Repeat sender with individual frame acknowledgment tracking |
| `reciever.c` | Selective Repeat receiver with out-of-order frame buffer |

## Compilation

```bash
gcc sender.c -o sr_sender
gcc reciever.c -o sr_receiver
```

# Go-Back-N (GBN) Sliding Window Protocol

Simulation of the Go-Back-N ARQ sliding window flow control protocol in C.

## Overview

Go-Back-N allows the sender to transmit up to $N$ frames before needing an acknowledgment (ACK):
- **Sender Window:** Size $N$. Frames within the window are transmitted in order.
- **Cumulative ACKs:** An ACK for frame $k$ acknowledges all frames up to $k$.
- **Timeout & Retransmission:** If a frame is lost or timer expires, the sender retransmits **all** unacknowledged frames starting from the lost frame (hence "Go-Back-N").
- **Receiver:** Discards out-of-order frames and only accepts strictly consecutive expected frames.

## Files

| File | Description |
|------|-------------|
| `sender.c` | GBN sender maintaining sliding window, timer, and retransmission logic |
| `reciever.c` | GBN receiver sending cumulative ACKs and checking expected sequence numbers |

## Compilation

```bash
gcc sender.c -o gbn_sender
gcc reciever.c -o gbn_receiver
```

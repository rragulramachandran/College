# Stop-and-Wait ARQ Protocol

Implementation of the Stop-and-Wait flow control and error control protocol in C.

## Overview

Stop-and-Wait is the simplest ARQ (Automatic Repeat reQuest) sliding window protocol:
- **Window Size:** Sender window = 1, Receiver window = 1.
- **Protocol Cycle:** Sender transmits a single packet and starts a timer. It waits for the receiver's acknowledgment before sending the next packet.
- **Sequence Numbers:** Alternates between 0 and 1 ($1$-bit sequence number) to prevent duplicate processing.
- **Loss Handling:** If the packet or ACK is lost, timeout expires and the sender retransmits the same packet.

## Files

| File | Description |
|------|-------------|
| `sender.c` | Stop-and-Wait sender with 1-bit sequence alternating logic |
| `reciever.c` | Stop-and-Wait receiver validating sequence numbers and returning ACKs |

## Compilation

```bash
gcc sender.c -o stop_wait_sender
gcc reciever.c -o stop_wait_receiver
```

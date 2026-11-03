# Model Lab 2 Practice Programs

Consolidated lab exam preparation code covering all key networking and socket programming modules in C.

## Contents

| Category | Programs | Description |
|----------|----------|-------------|
| **Framing Protocols** | `Bit.c`, `ppp.c` | Bit stuffing and Point-to-Point Protocol framing |
| **Error Detection & Correction** | `crc.c`, `crcSender.c`, `checksum.c`, `hammingCode.c`, `parity.c` | Cyclic Redundancy Check, 16-bit Internet Checksum, Hamming Code, 2D Parity |
| **Routing Algorithms** | `dijkstra.c`, `bellman.c` | Dijkstra link-state and Bellman-Ford distance-vector routing |
| **Iterative Sockets** | `itertcp.c`, `iterudp.c`, `cli.c`, `cliudp.c` | Sequential TCP and UDP client/server implementations |
| **Concurrent Sockets** | `conctcp.c`, `concudp.c`, `ser.c`, `serudp.c` | Multi-client concurrent TCP and UDP server architectures |

## Compilation

```bash
# Example: Dijkstra
gcc dijkstra.c -o dijkstra
./dijkstra

# Example: CRC
gcc crc.c -o crc
./crc
```

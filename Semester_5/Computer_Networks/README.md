# Computer Networks Lab

Laboratory programs for the Computer Networks course, covering network layering, data framing protocols, and error detection mechanisms.

All programs are written in **C**.

## Exercise List

| Exercise | Topic | Description |
|----------|-------|-------------|
| EX0 | Layering and Framing (DDCMP) | Network layer simulation with DNS hash table, IP/MAC resolution, and DDCMP byte-count framing |
| EX1 | Character Stuffing (BISYNC) | BISYNC character stuffing/destuffing with binary conversion, plus the full layering pipeline with DDCMP framing |
| EX2 | CRC Error Detection | Byte-count framing with CRC (Cyclic Redundancy Check) for error detection, modular sender-receiver design |
| EX3 | Error Correction & Detection | 2D Parity Check and Hamming Code error detection and correction |
| EX4 | Routing Algorithms | Shortest path routing simulation using Dijkstra's algorithm |
| EX5 | TCP Socket Programming | Client-server communication and socket API implementation for Linux and Windows |
| EX6 | ARP & File Transfer | Address Resolution Protocol (ARP) simulation, multi-client socket communication, and file transfer |
| EX7 | UDP Socket Applications | User Datagram Protocol (UDP) applications including DNS resolution, DHCP server, and peer chat |
| EX8 | Sliding Window Protocols | Flow control mechanisms including Stop-and-Wait (S&W) and Go-Back-N (GBN) with queue handling |

## Features

- **DNS Hash Table** — URL to IP/MAC address mapping using open-addressing hash table with CRUD operations
- **Network Layer Simulation** — Application, Transport, Network, and Data Link layer processing
- **BISYNC Framing** — Character stuffing and destuffing for control characters (DLE, STX, ETX, SOH, SYN)
- **DDCMP Framing** — Byte-count based framing with SYN synchronization and CRC validation
- **CRC Error Detection** — Polynomial division (mod-2) based error detection with optional error injection for testing
- **Error Correction** — Hamming code single-bit error detection/correction and 2D parity matrix verification
- **Routing** — Shortest path computation using Dijkstra's graph algorithm
- **Socket Programming** — TCP and UDP socket client-server architectures
- **Application Protocols** — Custom DNS lookup, DHCP IP lease allocation, and chat systems
- **Flow Control** — Stop-and-Wait and Go-Back-N sliding window transmission with queue management

## Compilation

**EX0 and EX1 (single file):**
```bash
gcc LayeringandFraming.c -o output
./output
```

**EX1 — BISYNC (single file):**
```bash
gcc BISYNC.c -o output
./output
```

**EX2 (multi-file):**
```bash
gcc Main.c Sender.c Reciever.c -o output
./output
```

## Input Files

- `message.txt` / `message1.txt` — Text message files used as input payload for framing and transmission

# Computer Networks Lab

Comprehensive laboratory repository for Computer Networks, covering physical-to-application layer protocols, data framing, error control, routing algorithms, socket programming (TCP & UDP), concurrent network servers, and NS-3 simulations.

All core programs are written in **C** using POSIX sockets, with network simulations in **C++ (NS-3)**.

## Complete Exercise Index

| Exercise / Folder | Topic | Description |
|-------------------|-------|-------------|
| [EX0](EX0/) | Layering & DDCMP Framing | Multi-tier network layer simulation with DNS hash table and DDCMP byte-count framing |
| [EX1](EX1/) | Framing Protocols & Stuffing | BISYNC character stuffing, DDCMP byte-count, HDLC bit stuffing, and PPP byte stuffing |
| [EX2](EX2/) | CRC Error Detection | Modular sender-receiver CRC implementation with modulo-2 polynomial division |
| [EX3](EX3/) | Error Detection & Correction | 2D Parity matrix verification and Hamming Code $(7, 4)$ single-bit correction |
| [EX4](EX4/) | Routing Algorithms | Dijkstra single-source shortest path and Bellman-Ford distance-vector routing |
| [EX5](EX5/) | Sockets Fundamentals | Core TCP and UDP client-server socket programming, arrays, and bit transfers |
| [EX6](EX6/) | Network Programming in C | 3 UDP services (Chat, DNS, DHCP) and 4 iterative TCP stateful systems (Railway, Hospital, Movie, Library) |
| [EX7](EX7/) | Concurrent TCP Systems | Multi-threaded TCP servers (ARP, Chat, File transfer, Exam, Hotel, File sharing, Notifications) |
| [EX8](EX8/) | Sliding Window Protocols | Stop-and-Wait and Go-Back-N sliding window transmission with queue management |
| [EX9](EX9/) | Concurrent TCP Server | Multi-process concurrent server handling simultaneous clients via `fork()` |
| [EX10](EX10/) | Network Simulator 3 (NS-3) | Discrete-event simulation of point-to-point topologies, queues, and throughput analysis |

## Dedicated Protocol Implementations

| Folder | Protocol / Application | Description |
|--------|------------------------|-------------|
| [ARP-TCP](ARP-TCP/) | ARP Protocol Simulation | TCP client-server resolving IP addresses to physical MAC addresses |
| [Current-Time-TCP](Current-Time-TCP/) | Daytime Service | Architecture benchmark: iterative vs fork-based concurrent daytime server |
| [FTP-TCP](FTP-TCP/) | File Transfer Protocol | Stream socket file download and upload system |
| [Go-Back-N](Go-Back-N/) | GBN Sliding Window | Go-Back-N flow control with cumulative acknowledgments and timeout retransmission |
| [Selective-Reapeat-TCP](Selective-Reapeat-TCP/) | Selective Repeat ARQ | Window-based selective retransmission with receiver buffering |
| [Stop-and-Wait-TCP](Stop-and-Wait-TCP/) | Stop-and-Wait ARQ | 1-bit sequence alternating flow control |
| [String-Operations-TCP](String-Operations-TCP/) | Remote String Services | Remote palindrome check, string reversal, and vowel counting |
| [Model2](Model2/) | Model Exam Practice | Consolidated exam code covering framing, error control, routing, and sockets |
| [Exercise_Questions](Exercise_Questions/) | Algorithmic Practice | C++ solutions for bit counting, pattern matching, and anagrams |

## Compilation Guide

### Framing & Layering (EX0, EX1)
```bash
gcc EX1/BISYNC.c -o bisync
gcc EX1/HDLC.c -o hdlc
gcc EX1/PPP.c -o ppp
```

### Error Control (EX2, EX3)
```bash
# CRC
cd EX2 && gcc Main.c Sender.c Reciever.c -o crc_app && ./crc_app

# Hamming Code
cd EX3 && gcc "sender.c(hamming code)" -o sender && gcc "receiver.c(hamming code)" -o receiver
```

### Routing Algorithms (EX4)
```bash
gcc EX4/DijkstraPractice.c -o dijkstra && ./dijkstra
gcc EX4/Bellman.c -o bellman && ./bellman
```

### Client-Server Sockets (EX5 - EX9)
```bash
# Start server in first terminal
gcc EX5/server.c -o server && ./server

# Run client in second terminal
gcc EX5/client.c -o client && ./client 127.0.0.1
```

# EX5: Socket Programming Fundamentals (TCP & UDP)

Foundational client-server socket programming using C and POSIX BSD Sockets.

## Overview

This exercise covers the core Berkeley Sockets API lifecycle:
- **TCP (Stream Sockets `SOCK_STREAM`):** Connection-oriented, reliable byte stream via `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `send()`, and `recv()`.
- **UDP (Datagram Sockets `SOCK_DGRAM`):** Connectionless, unreliable datagrams via `socket()`, `bind()`, `sendto()`, and `recvfrom()`.

## Files and Modules

| Category | Client | Server | Description |
|----------|--------|--------|-------------|
| **Echo / Basic** | `client.c` | `server.c` | Basic TCP echo messaging and string transfer |
| **Array Processing** | `Array_Client.c` | `Array_Server.c` | Sends integer arrays; server performs sorting/searching and returns results |
| **Bit Operations** | `Bit_Client.c` | `Bit_Server.c` | Bitwise encoding/decoding and casing transformation |
| **Multi-Service Menu** | `service_client.c` | `service_server.c` | Menu-driven service offering arithmetic, string, and status endpoints |
| **UDP Chat** | `UDP_Chat_Client.c` | `UDP_Chat_Server.c` | Two-way interactive UDP chat datagram application |

## Additional Documentation

- `Network_Functions_Syntax.md` — Quick reference of `sockaddr_in`, `inet_addr()`, `htons()`, socket syscalls, and error codes.
- `Network_Programs_QA.md` — Viva questions, architectural explanations, and common socket pitfalls.

## Compilation

```bash
# TCP Echo
gcc server.c -o tcp_server
gcc client.c -o tcp_client

# UDP Chat
gcc UDP_Chat_Server.c -o udp_server
gcc UDP_Chat_Client.c -o udp_client
```

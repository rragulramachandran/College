# EX9: Concurrent TCP Server

Implementation of a concurrent multi-client TCP server in C using multi-processing (`fork`).

## Overview

In contrast to iterative servers that block all other incoming connections while serving a single client, this concurrent server:
1. Listens on a well-known port using the parent process.
2. Calls `accept()` to establish incoming connection sockets.
3. Invokes `fork()` for every accepted connection:
   - **Child process:** Closes the listening socket and handles client requests independently.
   - **Parent process:** Closes the connected client descriptor and immediately loops back to `accept()` subsequent connections.

## Files

| File | Description |
|------|-------------|
| `server.c` | Multi-process concurrent TCP server handling multiple simultaneous client sessions |
| `client.c` | Interactive TCP client demonstrating concurrent connectivity |

## Compilation and Run

```bash
# Terminal 1: Start Server
gcc server.c -o concurrent_server
./concurrent_server

# Terminal 2 & Terminal 3: Connect Multiple Clients
gcc client.c -o client
./client 127.0.0.1 8080
```

# ARP Protocol Simulation over TCP

Client-server simulation of the Address Resolution Protocol (ARP) in C.

## Overview

Address Resolution Protocol (ARP) maps a dynamic IP address (Network Layer) to a physical MAC address (Data Link Layer).

In this simulation:
- The **server** maintains an in-memory table of IP-to-MAC address bindings.
- The **client** sends an IP query string to the server.
- The server performs table lookup; if found, it responds with the associated MAC address; otherwise, it reports ARP lookup failure.

## Files

| File | Description |
|------|-------------|
| `server.c` | TCP server holding IP-to-MAC resolution table |
| `client.c` | TCP client querying MAC addresses for requested IP destinations |

## Compilation and Execution

```bash
# Terminal 1: Run Server
gcc server.c -o arp_server
./arp_server

# Terminal 2: Run Client
gcc client.c -o arp_client
./arp_client
```

# Daytime Service over TCP (Iterative vs Concurrent)

Implementation of a TCP daytime / current-time service in C demonstrating the architectural differences between iterative and concurrent servers.

## Architecture Comparison

1. **Iterative Server (`server-iterative.c`):**
   - Single-threaded execution loop.
   - Accepts a client, computes current system timestamp via `time()` and `ctime()`, transmits the formatted string, and closes the connection before accepting the next client.
   - Any second client is queued in the socket backlog until the first finishes.

2. **Concurrent Server (`server-concurrent.c`):**
   - Forks a child process on every accepted connection (`fork()`).
   - The child process formats and transmits the time while the parent immediately accepts new connections.
   - Multiple clients receive simultaneous service without blocking.

## Files

| File | Description |
|------|-------------|
| `server-iterative.c` | Sequential iterative TCP daytime server |
| `server-concurrent.c` | Fork-based concurrent TCP daytime server |
| `client.c` | TCP client requesting and displaying server timestamps |

## Compilation

```bash
# Iterative server
gcc server-iterative.c -o time_iter
./time_iter

# Concurrent server
gcc server-concurrent.c -o time_conc
./time_conc

# Client
gcc client.c -o time_client
./time_client 127.0.0.1
```

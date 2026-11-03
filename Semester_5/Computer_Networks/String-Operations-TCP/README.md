# String Operations over TCP

Remote string processing client-server system in C using TCP sockets.

## Overview

Demonstrates remote procedural processing over stream sockets. The client sends strings along with operation selectors, and the server executes the requested string transformation and returns the result.

## Operations Supported

- **Palindrome Verification:** Checks whether a given string reads the same forwards and backwards.
- **String Reversal:** Inverts string characters.
- **Vowel and Consonant Count:** Analyzes and counts characters by type.
- **Case Conversion:** Toggles or transforms character cases (uppercase/lowercase).

## Files

| File | Description |
|------|-------------|
| `server.c` | TCP server executing requested string transformations |
| `client.c` | TCP client submitting strings and displaying transformed server outputs |

## Compilation

```bash
gcc server.c -o string_server
./string_server

gcc client.c -o string_client
./string_client 127.0.0.1
```

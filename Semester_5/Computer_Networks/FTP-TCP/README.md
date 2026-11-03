# File Transfer Protocol (FTP) over TCP

Client-server file download and upload system implemented in C over TCP sockets.

## Overview

A robust file transfer mechanism operating directly over raw stream sockets:
1. Client establishes connection and transmits the requested filename.
2. Server opens the file on local disk; if found, it transmits file content in chunks.
3. Client receives the byte stream and saves the file locally.
4. Error conditions such as file non-existence or permission errors are communicated gracefully.

## Files

| File | Description |
|------|-------------|
| `server.c` | TCP file server reading and streaming files to clients |
| `client.c` | TCP file client requesting filenames and writing downloaded bytes to disk |

## Compilation

```bash
gcc server.c -o ftp_server
./ftp_server

gcc client.c -o ftp_client
./ftp_client 127.0.0.1
```

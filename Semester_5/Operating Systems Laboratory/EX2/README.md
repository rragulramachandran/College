# EX2: Inter-Process Communication using Pipes

Inter-process communication (IPC) via unidirectional and bidirectional Unix pipes.

## Overview

A pipe provides a unidirectional byte stream buffer managed in kernel memory:
- `pipe(fd)` creates a pair of file descriptors: `fd[0]` for reading and `fd[1]` for writing.
- **Two-Way Communication:** Because standard pipes are half-duplex, bidirectional full-duplex communication requires **two distinct pipes**:
  - Pipe 1: Parent writes $\rightarrow$ Child reads.
  - Pipe 2: Child writes $\rightarrow$ Parent reads.

## Files

| File | Description |
|------|-------------|
| `two_way_pipe.c` | Full-duplex two-way communication between parent and child processes |
| `process_tree.c` | IPC coordination across a tree of processes |

## Compilation

```bash
gcc two_way_pipe.c -o two_way_pipe
./two_way_pipe
```

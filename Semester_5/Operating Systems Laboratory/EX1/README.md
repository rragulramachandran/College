# EX1: Process Creation and Hierarchies

Process management in Unix/Linux using the `fork()` system call.

## Overview

Demonstrates how the operating system manages process creation, Parent-Child relationships, process IDs (`getpid()`, `getppid()`), and termination synchronization (`wait()`):
- **Linear Hierarchy:** Each process creates exactly one child, forming a sequential ancestor chain: $P_1 \rightarrow P_2 \rightarrow P_3 \rightarrow \dots \rightarrow P_n$.
- **Tree Hierarchy:** A parent process spawns multiple child processes, each of which in turn spawns sub-children, creating a hierarchical process tree.

## Files

| File | Description |
|------|-------------|
| `LinearHierarchy.c` | Creates a linear sequential chain of processes |
| `TreeHierarchy1.c` | Binary tree process creation pattern |
| `TreeHierarchy2.c` | Multi-level hierarchical tree structure |
| `Tree.txt` | ASCII visualization of the expected process tree |

## Compilation

```bash
gcc LinearHierarchy.c -o linear && ./linear
gcc TreeHierarchy1.c -o tree1 && ./tree1
```

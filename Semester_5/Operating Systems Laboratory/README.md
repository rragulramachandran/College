# Operating Systems Laboratory

Laboratory programs for the Operating Systems course, implementing fundamental OS kernel mechanisms including process hierarchies, inter-process communication (IPC), CPU scheduling, process synchronization, deadlock avoidance, memory management, page replacement, and disk scheduling.

All programs are implemented in **C** using POSIX system calls (`fork`, `pipe`, `msgget`, `shmget`, `semget`, `pthread`).

## Exercise List

| Exercise | Topic | Concepts | Description |
|----------|-------|----------|-------------|
| [EX1](EX1/) | Process Creation & Hierarchies | `fork()`, `getpid()`, `getppid()`, `wait()` | Linear process chains and binary tree process hierarchies |
| [EX2](EX2/) | Inter-Process Communication: Pipes | `pipe()`, `read()`, `write()`, file descriptors | Full-duplex two-way communication between parent and child processes |
| [EX3](EX3/) | IPC: Message Queues & Shared Memory | System V IPC (`msgget`, `msgsnd`, `msgrcv`, `shmget`, `shmat`) | Message passing queues and shared memory matrix operations / string reversals |
| [EX4](EX4/) | CPU Scheduling Algorithms | Preemptive & Non-preemptive scheduling | FCFS, SJF, Priority Scheduling, and Round Robin (RR) with turnaround & waiting times |
| [EX5](EX5/) | Process Synchronization (Semaphores) | Counting & Binary Semaphores | Producer-Consumer problem solving mutual exclusion and buffer coordination |
| [EX6](EX6/) | Deadlock Avoidance | Banker's Algorithm | Safety state verification and Resource Request algorithm on allocation matrices |
| [EX7](EX7/) | Contiguous Memory Allocation | Memory partitioning | First Fit, Best Fit, and Worst Fit dynamic memory placement strategies |
| [EX8](EX8/) | Page Replacement Algorithms | Virtual memory management | FIFO, Least Recently Used (LRU), and Optimal page replacement with hit/fault ratios |
| [EX11](EX11/) | Disk Scheduling Algorithms | Secondary storage optimization | FCFS, SSTF, SCAN, C-SCAN, LOOK, and C-LOOK head movement minimization |

## Build and Run

Compile any exercise with `gcc`:

```bash
# Process Creation (EX1)
gcc EX1/LinearHierarchy.c -o linear && ./linear
gcc EX1/TreeHierarchy1.c -o tree1 && ./tree1

# Two-Way Pipe (EX2)
gcc EX2/two_way_pipe.c -o pipe && ./pipe

# CPU Scheduling (EX4)
cd EX4 && gcc main.c scheduling_algorithms.c -o sched && ./sched

# Banker's Algorithm (EX6)
gcc EX6/bankersAlgorithm.c -o bankers && ./bankers

# Memory Allocation (EX7)
gcc EX7/continuos_memory_allocation.c -o mem_alloc && ./mem_alloc

# Page Replacement (EX8)
gcc EX8/pageReplacementAlgorithms.c -o page_replace && ./page_replace

# Disk Scheduling (EX11)
gcc EX11/diskSchedulingAlgorithms.c -o disk_sched && ./disk_sched
```

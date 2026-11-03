# EX4: CPU Scheduling Algorithms

Modular implementation and performance evaluation of standard CPU scheduling algorithms in C.

## Overview

CPU scheduling decides which process in the ready queue is allocated the CPU core. This module computes **Turnaround Time (TAT)**, **Waiting Time (WT)**, and **Response Time**, along with system averages.

## Algorithms Implemented

1. **First Come First Served (FCFS):** Non-preemptive scheduling in order of arrival.
2. **Shortest Job First (SJF):** Selects the process with the smallest CPU burst time (minimizes average waiting time).
3. **Priority Scheduling:** Prioritizes processes based on assigned priority integers.
4. **Round Robin (RR):** Preemptive time-sharing scheduling allocating a fixed Time Quantum ($q$) to each process cyclically.

## Files

| File | Description |
|------|-------------|
| `scheduling.h` | Process structure definition (`pid`, arrival, burst, priority, completion, TAT, WT) |
| `scheduling_algorithms.c` | Core logic for FCFS, SJF, Priority, and Round Robin scheduling |
| `main.c` | Interactive CLI menu accepting process tables and displaying scheduling metrics |

## Compilation

```bash
gcc main.c scheduling_algorithms.c -o scheduler
./scheduler
```

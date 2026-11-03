# EX11: Disk Scheduling Algorithms

Simulation of magnetic hard disk head scheduling algorithms in C.

## Overview

Disk scheduling algorithms optimize the order in which pending I/O track requests are serviced, minimizing total disk arm seek time and head movement distance.

## Algorithms Implemented

1. **FCFS (First-Come, First-Served):** Services requests strictly in their arrival order.
2. **SSTF (Shortest Seek Time First):** Services the request closest to the current head position (may lead to starvation).
3. **SCAN (Elevator Algorithm):** Arm moves towards one disk boundary, servicing requests along the way, then reverses direction.
4. **C-SCAN (Circular SCAN):** Moves in one direction servicing requests; upon hitting the boundary, jumps immediately back to the beginning without servicing requests on the return trip.
5. **LOOK:** Like SCAN, but reverses direction immediately after servicing the last request in that direction, without traveling all the way to the disk edge.
6. **C-LOOK:** Circular variant of LOOK that jumps directly to the lowest requested cylinder.

## Compilation

```bash
gcc diskSchedulingAlgorithms.c -o disk_sched
./disk_sched
```

# EX6: Deadlock Avoidance (Banker's Algorithm)

Implementation of Dijkstra's Banker's Algorithm for deadlock avoidance in multi-resource systems.

## Overview

The Banker's Algorithm evaluates resource requests by testing whether granting the request leaves the system in a **Safe State** (a sequence $\langle P_0, P_1, \dots, P_{n-1} \rangle$ exists such that every process can finish).

### Matrices Used
- **Allocation Matrix ($A_{n \times m}$):** Resources currently allocated to each process.
- **Max Matrix ($M_{n \times m}$):** Maximum resource demand of each process.
- **Available Vector ($V_m$):** Currently free instances of each resource type.
- **Need Matrix ($N_{n \times m}$):** Remaining resources needed: $N_{i,j} = M_{i,j} - A_{i,j}$.

## Key Routines

1. **Safety Algorithm:** Determines whether the current allocation state is safe and outputs the valid safe sequence.
2. **Resource-Request Algorithm:** Tests whether a dynamic request from process $P_i$ can be safely granted without inducing deadlock.

## Compilation

```bash
gcc bankersAlgorithm.c -o bankers
./bankers
```

# EX7: Contiguous Memory Allocation

Implementation of dynamic memory partition allocation strategies in C.

## Overview

Simulates how an operating system allocates contiguous physical memory blocks of varying sizes to incoming processes.

## Allocation Strategies Implemented

1. **First Fit:** Allocates the *first* hole in the memory block list that is large enough ($\text{size} \ge \text{process\_size}$). Fast and computationally efficient.
2. **Best Fit:** Searches the *entire* list and allocates the smallest hole that is big enough. Minimizes immediate leftover space, but may create many tiny, unusable fragments.
3. **Worst Fit:** Allocates the *largest* available hole, leaving the largest remaining fragment that may be large enough to host future processes.

## Features

- Dynamic partition table tracking
- Internal and external fragmentation calculation
- Process allocation and deallocation lifecycle simulation

## Compilation

```bash
gcc continuos_memory_allocation.c -o mem_alloc
./mem_alloc
```

# EX8: Page Replacement Algorithms

Simulation of virtual memory page replacement algorithms in C.

## Overview

When a process references a virtual page not currently resident in physical memory frames, a **Page Fault** occurs. If all allocated page frames are full, a page replacement algorithm chooses which victim page to evict.

## Algorithms Implemented

1. **First-In, First-Out (FIFO):**
   - Evicts the oldest page brought into memory.
   - Susceptible to Belady's Anomaly (page faults increasing as frame allocation increases).

2. **Least Recently Used (LRU):**
   - Evicts the page that has not been referenced for the longest period of time.
   - Approximates optimal replacement using backward temporal locality.

3. **Optimal Page Replacement (OPT / MIN):**
   - Evicts the page that will not be used for the longest period of time in the future.
   - Serves as the theoretical performance upper bound.

## Performance Metrics

- Total Page Faults ($F$)
- Total Page Hits ($H$)
- Fault Ratio: $\frac{F}{N} \times 100\%$
- Hit Ratio: $\frac{H}{N} \times 100\%$

## Compilation

```bash
gcc pageReplacementAlgorithms.c -o page_replace
./page_replace
```

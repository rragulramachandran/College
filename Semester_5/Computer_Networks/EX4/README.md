# EX4: Routing Algorithms (Dijkstra & Bellman-Ford)

Implementation of Shortest Path and Distance-Vector routing algorithms in C.

## Overview

Routing algorithms determine optimal packet forwarding paths across networked routers represented as weighted graphs.

### Algorithms Implemented

1. **Dijkstra's Algorithm (Link-State Routing):**
   - Computes single-source shortest path using a greedy strategy.
   - Maintains visited state, tentative distance array, and predecessor pointers.
   - Complexity: $O(V^2)$ with adjacency matrix.

2. **Bellman-Ford Algorithm (Distance-Vector Routing):**
   - Iteratively relaxes all edges $|V| - 1$ times.
   - Supports distributed distance-vector routing (RIP) and detects negative weight cycles.
   - Complexity: $O(V \cdot E)$.

## Files

| File | Description |
|------|-------------|
| `DijkstraPractice.c` | Dijkstra shortest path tree computation with full routing table display |
| `Bellman.c` / `BellmanPractice.c` | Bellman-Ford distance-vector routing and negative cycle detection |

## Compilation

```bash
# Dijkstra
gcc DijkstraPractice.c -o dijkstra
./dijkstra

# Bellman-Ford
gcc Bellman.c -o bellman
./bellman
```

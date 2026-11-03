Routing Algorithms - Simple Codes
=================================
bellman_ford_simple.c  -> Bellman-Ford (edge list, handles negative weights, detects negative cycle)
dijkstra_simple.c      -> Dijkstra (adjacency matrix, positive weights only)

Compile and run:
  gcc bellman_ford_simple.c -o bf && ./bf
  gcc dijkstra_simple.c -o dj && ./dj

Sample input (both programs):
  5 nodes, 6 edges: 0 1 4 / 0 2 1 / 2 1 2 / 1 3 1 / 2 3 5 / 3 4 3, source 0

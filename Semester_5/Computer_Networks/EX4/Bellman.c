#include <stdio.h>
#include <stdlib.h>

#define INF 99999
#define MAX 20

int adj[MAX][MAX];
int V;

// STACK DATA STRUCTURE
int stack[MAX];
int top = -1;

void push(int val) {
  if (top < MAX - 1) {
    stack[++top] = val;
  }
}

int pop(void) {
  if (top >= 0) {
    return stack[top--];
  }
  return -1;
}

struct PathInfo {
  int nodes[MAX];
  int length;
  int cost;
};

struct PathInfo allPaths[MAX][100];
int pathCount[MAX];

void findAllPaths(int u, int visited[]) {
  visited[u] = 1;
  push(u);

  int currentCost = 0;
  for (int i = 0; i < top; i++) {
    currentCost += adj[stack[i]][stack[i + 1]];
  }

  int dest = u;
  int count = pathCount[dest];
  allPaths[dest][count].length = top + 1;
  allPaths[dest][count].cost = currentCost;

  for (int i = 0; i <= top; i++) {
    allPaths[dest][count].nodes[i] = stack[i];
  }

  pathCount[dest]++;

  for (int v = 0; v < V; v++) {
    if (adj[u][v] != INF && !visited[v]) {
      findAllPaths(v, visited);
    }
  }

  pop();
  visited[u] = 0;
}

void printAllAndShortestPaths(int source) {
  int visited[MAX] = {0};

  top = -1;
  for (int i = 0; i < V; i++) {
    pathCount[i] = 0;
  }

  findAllPaths(source, visited);

  printf("\n=======================================================\n");
  printf("         ALL POSSIBLE PATHS & SHORTEST SELECTION       \n");
  printf("=======================================================\n");

  for (int dest = 0; dest < V; dest++) {
    printf("\nDESTINATION VERTEX: %d\n", dest);

    if (pathCount[dest] == 0) {
      printf("No path exists from %d to %d.\n", source, dest);
      continue;
    }

    printf("\nAll Possible Paths:\n");
    int minCost = INF;
    int minIndex = -1;

    for (int i = 0; i < pathCount[dest]; i++) {
      printf("  Path %d: ", i + 1);
      for (int j = 0; j < allPaths[dest][i].length; j++) {
        printf("%d", allPaths[dest][i].nodes[j]);
        if (j < allPaths[dest][i].length - 1)
          printf(" -> ");
      }
      printf(" | Cost = %d\n", allPaths[dest][i].cost);

      if (allPaths[dest][i].cost < minCost) {
        minCost = allPaths[dest][i].cost;
        minIndex = i;
      }
    }

    printf("\nSELECTED SHORTEST PATH: ");
    for (int j = 0; j < allPaths[dest][minIndex].length; j++) {
      printf("%d", allPaths[dest][minIndex].nodes[j]);
      if (j < allPaths[dest][minIndex].length - 1)
        printf(" -> ");
    }
    printf(" (Min Cost = %d)\n", minCost);
    printf("-------------------------------------------------------\n");
  }
}

int main(void) {
  int E, source;

  printf("Enter number of vertices (|V|): ");
  if (scanf("%d", &V) != 1 || V <= 0 || V > MAX)
    return 1;

  for (int i = 0; i < V; i++) {
    for (int j = 0; j < V; j++) {
      adj[i][j] = INF;
    }
  }

  printf("Enter number of edges (|E|): ");
  if (scanf("%d", &E) != 1 || E <= 0)
    return 1;

  printf("\nEnter edges in the format (u v w):\n");
  for (int i = 0; i < E; i++) {
    int u, v, w;
    printf("Edge %d (u v w): ", i + 1);
    if (scanf("%d %d %d", &u, &v, &w) == 3) {
      adj[u][v] = w;
    }
  }

  printf("\nEnter source vertex 's': ");
  if (scanf("%d", &source) != 1 || source < 0 || source >= V)
    return 1;

  printAllAndShortestPaths(source);

  return 0;
}
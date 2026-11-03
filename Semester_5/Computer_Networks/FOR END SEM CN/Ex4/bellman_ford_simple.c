#include <stdio.h>

#define INF 999999
#define MAX 20

int main() {
    int V, E, s, i, j;
    int from[MAX * MAX], to[MAX * MAX], w[MAX * MAX];
    int dist[MAX], parent[MAX];

    /* 1. Input */
    printf("Enter number of nodes: ");
    scanf("%d", &V);
    printf("Enter number of edges: ");
    scanf("%d", &E);
    printf("Enter each edge as: from to weight (weight can be negative)\n");
    for (i = 0; i < E; i++) {
        printf("Edge %d: ", i + 1);
        scanf("%d %d %d", &from[i], &to[i], &w[i]);
    }
    printf("Enter source node: ");
    scanf("%d", &s);

    /* 2. Initialize */
    for (i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[s] = 0;

    /* 3. Relax all edges (V-1) times */
    for (i = 1; i <= V - 1; i++) {
        for (j = 0; j < E; j++) {
            int u = from[j], v = to[j];
            if (dist[u] != INF && dist[u] + w[j] < dist[v]) {
                dist[v] = dist[u] + w[j];
                parent[v] = u;
            }
        }
    }

    /* 4. One more pass: any update means a negative cycle */
    for (j = 0; j < E; j++) {
        int u = from[j], v = to[j];
        if (dist[u] != INF && dist[u] + w[j] < dist[v]) {
            printf("\nNegative weight cycle exists! No shortest path.\n");
            return 0;
        }
    }

    /* 5. Routing table */
    printf("\nRouting table from node %d\n", s);
    printf("Dest\tCost\tNextHop\tPath\n");
    for (i = 0; i < V; i++) {
        if (dist[i] == INF) {
            printf("%d\tINF\t-\tunreachable\n", i);
            continue;
        }

        /* build path by walking back through parent[] */
        int path[MAX], len = 0, x = i;
        while (x != -1) {
            path[len++] = x;
            x = parent[x];
        }

        /* path[len-1] is the source, path[len-2] is the next hop */
        printf("%d\t%d\t", i, dist[i]);
        if (i == s) printf("-\t");
        else        printf("%d\t", path[len - 2]);

        for (j = len - 1; j >= 0; j--) {
            printf("%d", path[j]);
            if (j > 0) printf(" -> ");
        }
        printf("\n");
    }
    return 0;
}

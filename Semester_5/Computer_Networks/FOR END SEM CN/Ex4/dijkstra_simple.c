#include <stdio.h>

#define INF 999999
#define MAX 20

int main() {
    int V, E, s, i, j;
    int g[MAX][MAX];                 /* adjacency matrix, 0 = no edge */
    int dist[MAX], parent[MAX], visited[MAX];

    /* 1. Input */
    printf("Enter number of nodes: ");
    scanf("%d", &V);
    for (i = 0; i < V; i++)
        for (j = 0; j < V; j++)
            g[i][j] = 0;

    printf("Enter number of edges: ");
    scanf("%d", &E);
    printf("Enter each edge as: from to weight (positive only, undirected)\n");
    for (i = 0; i < E; i++) {
        int u, v, wt;
        printf("Edge %d: ", i + 1);
        scanf("%d %d %d", &u, &v, &wt);
        g[u][v] = wt;
        g[v][u] = wt;
    }
    printf("Enter source node: ");
    scanf("%d", &s);

    /* 2. Initialize */
    for (i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
        visited[i] = 0;
    }
    dist[s] = 0;

    /* 3. Repeat V times: pick closest unvisited node, relax its neighbours */
    for (i = 0; i < V; i++) {
        int u = -1;
        for (j = 0; j < V; j++)
            if (!visited[j] && (u == -1 || dist[j] < dist[u]))
                u = j;

        if (dist[u] == INF) break;   /* remaining nodes unreachable */
        visited[u] = 1;

        for (j = 0; j < V; j++) {
            if (g[u][j] != 0 && !visited[j] && dist[u] + g[u][j] < dist[j]) {
                dist[j] = dist[u] + g[u][j];
                parent[j] = u;
            }
        }
    }

    /* 4. Routing table */
    printf("\nRouting table from node %d\n", s);
    printf("Dest\tCost\tNextHop\tPath\n");
    for (i = 0; i < V; i++) {
        if (dist[i] == INF) {
            printf("%d\tINF\t-\tunreachable\n", i);
            continue;
        }

        int path[MAX], len = 0, x = i;
        while (x != -1) {
            path[len++] = x;
            x = parent[x];
        }

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

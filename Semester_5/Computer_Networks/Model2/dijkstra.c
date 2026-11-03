
#include <stdio.h>
#define INF 99999

int main()
{
    int V, s, i, j, u, min;

    printf("Enter number of vertices: ");
    scanf("%d", &V);

    int a[V][V], d[V], visited[V], nh[V];

    printf("Enter adjacency matrix (0 for no edge):\n");
    for (i = 0; i < V; i++)
        for (j = 0; j < V; j++)
        {
            scanf("%d", &a[i][j]);
            if (i != j && a[i][j] == 0)
                a[i][j] = INF;
        }

    printf("Enter source: ");
    scanf("%d", &s);

    for (i = 0; i < V; i++)
    {
        d[i] = a[s][i];
        visited[i] = 0;
        nh[i] = (i != s && d[i] != INF) ? i : -1;
    }

    d[s] = 0;
    visited[s] = 1;

    for (i = 1; i < V; i++)
    {
        min = INF;
        u = -1;

        for (j = 0; j < V; j++)
            if (!visited[j] && d[j] < min)
            {
                min = d[j];
                u = j;
            }

        if (u == -1)
            break;

        visited[u] = 1;

        for (j = 0; j < V; j++)
            if (!visited[j] && a[u][j] != INF &&
                d[u] + a[u][j] < d[j])
            {
                d[j] = d[u] + a[u][j];
                nh[j] = (u == s) ? j : nh[u];
            }

        printf("\nIteration %d\n", i);
        printf("Vertex\tDistance\tNext Hop\n");

        for (j = 0; j < V; j++)
        {
            if (d[j] == INF)
                printf("%d\tINF\t\t-\n", j);
            else
                printf("%d\t%d\t\t%d\n",
                       j, d[j], j == s ? -1 : nh[j]);
        }
    }

    printf("\nFinal Results\n");
    printf("Vertex\tDistance\tNext Hop\n");

    for (i = 0; i < V; i++)
    {
        if (d[i] == INF)
            printf("%d\tINF\t\t-\n", i);
        else
            printf("%d\t%d\t\t%d\n",
                   i, d[i], i == s ? -1 : nh[i]);
    }

    return 0;
}

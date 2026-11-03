#include <stdio.h>
#define INF 99999

struct Edge
{
    int u, v, w;
};

int main()
{
    int V, E, s, i, j;

    printf("Enter vertices and edges: ");
    scanf("%d %d", &V, &E);

    struct Edge e[E];
    int d[V], p[V], nh[V];

    printf("Enter edges (u v weight):\n");
    for (i = 0; i < E; i++)
        scanf("%d %d %d", &e[i].u, &e[i].v, &e[i].w);

    printf("Enter source: ");
    scanf("%d", &s);

    for (i = 0; i < V; i++)
    {
        d[i] = INF;
        p[i] = -1;
        nh[i] = -1;
    }

    d[s] = 0;
    p[s] = s;

    for (i = 1; i < V; i++)
    {
        for (j = 0; j < E; j++)
        {
            int u = e[j].u, v = e[j].v, w = e[j].w;

            if (d[u] != INF && d[u] + w < d[v])
            {
                d[v] = d[u] + w;
                p[v] = u;

                if (u == s)
                    nh[v] = v;
                else
                    nh[v] = nh[u];
            }
        }

        printf("\nIteration %d\n", i);
        printf("Vertex\tDistance\tNext Hop\n");

        for (j = 0; j < V; j++)
        {
            if (d[j] == INF)
                printf("%d\tINF\t\t-\n", j);
            else
                printf("%d\t%d\t\t%d\n", j, d[j],
                       j == s ? -1 : nh[j]);
        }
    }

    for (i = 0; i < E; i++)
    {
        int u = e[i].u, v = e[i].v, w = e[i].w;

        if (d[u] != INF && d[u] + w < d[v])
        {
            printf("\nNegative weight cycle exists!\n");
            return 0;
        }
    }

    printf("\nFinal Results\n");
    printf("Vertex\tDistance\tNext Hop\n");

    for (i = 0; i < V; i++)
    {
        if (d[i] == INF)
            printf("%d\tINF\t\t-\n", i);
        else
            printf("%d\t%d\t\t%d\n", i, d[i],
                   i == s ? -1 : nh[i]);
    }

    return 0;
}

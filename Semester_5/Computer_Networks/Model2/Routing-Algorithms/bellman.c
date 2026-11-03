#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define INF 999

struct Edge
{
    int u, v, w;
};

void printPath(int p[], int v)
{
    if(p[v] == v)
    {
        printf("%d", p[v]);
        return ;
    }

    if(p[v] == -1)
    {
        return ;
    }

    printPath(p, p[v]);
    printf(" -> %d", v);
}

int main()
{

    int V, E, s;
    printf("Enter No. of Vertices: ");
    scanf("%d", &V);

    printf("Enter No. of Edges: ");
    scanf("%d", &E);

    struct Edge e[E];
    int d[V], p[V], nh[V];

    for(int i = 0; i < E; i++)
    {
        printf("Enter Source, Destination, Weight : ");
        scanf("%d %d %d", &e[i].u,&e[i].v, &e[i].w);
    }

    for(int i = 0; i < V; i++)
    {
        d[i] = INF;
        p[i] = -1;
        nh[i] = -1;
    }

    printf("Enter Source Vertex : ");
    scanf("%d", &s);
    printf("\n");

    d[s] = 0;
    p[s] = s;

    for(int i = 0; i < V; i++)
    {
        for(int j = 0; j < E; j++)
        {
            int u = e[j].u;
            int v = e[j].v;
            int w = e[j].w;

            if(d[u] != INF && d[u] + w < d[v])
            {
                d[v] = d[u] + w;
                p[v] = u;

                if(u == s)
                {
                    nh[v] = v;
                }
                else
                {
                    nh[v] = nh[u];
                }
            }
        }
    }

    for(int j = 0; j < E; j++)
    {
        int u = e[j].u;
            int v = e[j].v;
            int w = e[j].w;

            if(d[u] != INF && d[u] + w < d[v])
            {
                printf("Error : Negative Cycle Detected\n");
                return 0;
            }
    }

    printf("Dest\tDist\tParent\tNext Hop\n");

    for(int i = 0; i < V; i++)
    {
        printf("%d\t%d\t%d\t%d\n",i, d[i], p[i], nh[i]);
    }

    printf("\n");


    for(int i = 0; i < V; i++)
    {
        printf("%d to %d : ", s, i);
        
        if(d[i] == INF)
        {
            printf("No Path\n");
        }
        else
        {
            printPath(p, i);
            printf(" Cost : %d\n", d[i]);
        }
    }

    return 0;
}
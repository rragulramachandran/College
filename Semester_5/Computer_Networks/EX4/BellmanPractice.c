#include <stdio.h>

struct Edge
{
    int source;
    int destination;
    int weight;
};

int V;
int E;

void printPath(int parent[], int vertex)
{
    if(parent[vertex] == -1)
    {
        printf("%d", vertex);
        return;
    }

    printPath(parent, parent[vertex]);
    printf(" -> %d", vertex);
}

int main()
{
    printf("Enter No. Of Vertices : ");
    scanf("%d", &V);

    printf("Enter No. Of Edges : ");
    scanf("%d", &E);

    struct Edge edges[E];

    printf("\nEnter Details of the Edges:\n");

    for(int i = 0; i < E; i++)
    {
        printf("\nEnter Edge %d's Source Vertex : ", i);
        scanf("%d", &edges[i].source);

        printf("Enter Edge %d's Destination Vertex : ", i);
        scanf("%d", &edges[i].destination);

        printf("Enter Edge %d's Weight : ", i);
        scanf("%d", &edges[i].weight);
    }

    int distance[V];
    int parent[V];
    int src;

    printf("\nEnter Source Vertex : ");
    scanf("%d", &src);

    for(int i = 0; i < V; i++)
    {
        distance[i] = 99;
        parent[i] = -1;
    }

    distance[src] = 0;

    for(int i = 0; i < V - 1; i++)
    {
        for(int j = 0; j < E; j++)
        {
            if(distance[edges[j].source] != 99 &&
               distance[edges[j].source] + edges[j].weight <
               distance[edges[j].destination])
            {
                distance[edges[j].destination] =
                    distance[edges[j].source] + edges[j].weight;

                parent[edges[j].destination] =
                    edges[j].source;
            }
        }
    }

    int negativeCycle = 0;

    for(int j = 0; j < E; j++)
    {
        if(distance[edges[j].source] != 99 &&
           distance[edges[j].source] + edges[j].weight <
           distance[edges[j].destination])
        {
            negativeCycle = 1;
            break;
        }
    }

    if(negativeCycle)
    {
        printf("\nNegative Weight Cycle Detected!\n");
    }
    else
    {
        printf("\nShortest Distances and Paths:\n\n");

        for(int i = 0; i < V; i++)
        {
            printf("Vertex %d : Distance = %d | Path = ",
                   i, distance[i]);

            if(distance[i] == 99)
            {
                printf("Unreachable");
            }
            else
            {
                printPath(parent, i);
            }

            printf("\n");
        }
    }

    return 0;
}
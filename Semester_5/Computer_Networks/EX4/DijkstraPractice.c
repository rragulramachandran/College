#include <stdio.h>
#include <stdlib.h>

struct Node
{
    int vertex;
    int weight;
    struct Node* next;
};

struct HeapNode
{
    int vertex;
    int distance;
};

struct MinHeap
{
    struct HeapNode* array;
    int size;
    int capacity;
};

int V;
int E;

void printAdjList(struct Node* adj[])
{
    for(int i = 0; i < V; i++)
    {
        printf("Vertex %d ", i);

        struct Node* temp = adj[i];

        while(temp != NULL)
        {
            printf("-> [%d, %d] ", temp->vertex, temp->weight);
            temp = temp->next;
        }

        printf("\n");
    }
}

struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap = malloc(sizeof(struct MinHeap));

    minHeap->array = malloc(capacity * sizeof(struct HeapNode));

    minHeap->size = 0;
    minHeap->capacity = capacity;

    return minHeap;
}

void swap(struct HeapNode* a, struct HeapNode* b)
{
    struct HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(struct MinHeap* minHeap, int index)
{
    int parent = (index - 1) / 2;

    while(index > 0 && minHeap->array[index].distance < minHeap->array[parent].distance)
    {
        swap(&minHeap->array[index],
             &minHeap->array[parent]);

        index = parent;
        parent = (index - 1) / 2;
    }
}

void insertMinHeap(struct MinHeap* minHeap, int vertex, int distance)
{
    if(minHeap->size == minHeap->capacity)
    {
        minHeap->capacity *= 2;

        minHeap->array = realloc(
            minHeap->array,
            minHeap->capacity * sizeof(struct HeapNode)
        );
    }

    int index = minHeap->size;

    minHeap->array[index].vertex = vertex;
    minHeap->array[index].distance = distance;

    minHeap->size++;

    heapifyUp(minHeap, index);
}

void heapifyDown(struct MinHeap* minHeap, int index)
{
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if(left < minHeap->size && minHeap->array[left].distance < minHeap->array[smallest].distance)
    {
        smallest = left;
    }

    if(right < minHeap->size && minHeap->array[right].distance < minHeap->array[smallest].distance)
    {
        smallest = right;
    }

    if(smallest != index)
    {
        swap(&minHeap->array[index], &minHeap->array[smallest]);

        heapifyDown(minHeap, smallest);
    }
}

struct HeapNode extractMin(struct MinHeap* minHeap)
{
    struct HeapNode minNode = minHeap->array[0];

    minHeap->array[0] = minHeap->array[minHeap->size - 1];

    minHeap->size--;

    if(minHeap->size > 0)
    {
        heapifyDown(minHeap, 0);
    }

    return minNode;
}

int main()
{
    printf("Enter No. Of Vertices : ");
    scanf("%d", &V);

    printf("Enter No. Of Edges : ");
    scanf("%d", &E);

    struct Node* adj[V];

    for(int i = 0; i < V; i++)
    {
        adj[i] = NULL;
    }

    for(int i = 0; i < E; i++)
    {
        int source, destination, weight;

        printf("\nEnter Edge %d's Source Vertex : ", i);
        scanf("%d", &source);

        printf("Enter Edge %d's Destination Vertex : ", i);
        scanf("%d", &destination);

        printf("Enter Edge %d's Weight : ", i);
        scanf("%d", &weight);

        struct Node* newNode =
            malloc(sizeof(struct Node));

        newNode->vertex = destination;
        newNode->weight = weight;
        newNode->next = adj[source];

        adj[source] = newNode;
    }

    printf("\n");
    printAdjList(adj);

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

    struct MinHeap* minHeap = createMinHeap(V);

    insertMinHeap(minHeap, src, 0);

    return 0;
}
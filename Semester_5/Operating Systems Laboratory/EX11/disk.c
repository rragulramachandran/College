#include <stdio.h>
#include <stdlib.h>

int absVal(int x)
{
    return (x < 0) ? -x : x;
}

void sort(int a[], int n)
{
    int i, j, temp;

    for(i = 0; i < n - 1; i++)
    {
        for(j = 0; j < n - i - 1; j++)
        {
            if(a[j] > a[j + 1])
            {
                temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }
        }
    }
}

/* FCFS */
void FCFS(int req[], int n, int head)
{
    int total = 0;

    printf("\nFCFS Order : %d", head);

    for(int i = 0; i < n; i++)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    printf("\nTotal Head Movement = %d\n", total);
}

/* SSTF */
void SSTF(int req[], int n, int head)
{
    int visited[20] = {0};
    int total = 0;

    printf("\nSSTF Order : %d", head);

    for(int i = 0; i < n; i++)
    {
        int min = 9999;
        int index = -1;

        for(int j = 0; j < n; j++)
        {
            if(!visited[j] && absVal(req[j] - head) < min)
            {
                min = absVal(req[j] - head);
                index = j;
            }
        }

        visited[index] = 1;
        total += min;
        head = req[index];

        printf(" -> %d", head);
    }

    printf("\nTotal Head Movement = %d\n", total);
}

/* SCAN */
void SCAN(int req[], int n, int head, int size)
{
    sort(req, n);

    int total = 0;
    int pos;

    for(pos = 0; pos < n; pos++)
        if(req[pos] > head)
            break;

    printf("\nSCAN Order : %d", head);

    for(int i = pos; i < n; i++)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    total += absVal((size - 1) - head);
    head = size - 1;

    for(int i = pos - 1; i >= 0; i--)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    printf("\nTotal Head Movement = %d\n", total);
}

/* C-SCAN */
void CSCAN(int req[], int n, int head, int size)
{
    sort(req, n);

    int total = 0;
    int pos;

    for(pos = 0; pos < n; pos++)
        if(req[pos] > head)
            break;

    printf("\nC-SCAN Order : %d", head);

    for(int i = pos; i < n; i++)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    total += absVal((size - 1) - head);
    total += size - 1;

    head = 0;

    for(int i = 0; i < pos; i++)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    printf("\nTotal Head Movement = %d\n", total);
}

/* LOOK */
void LOOK(int req[], int n, int head)
{
    sort(req, n);

    int total = 0, pos;

    for(pos = 0; pos < n; pos++)
        if(req[pos] > head)
            break;

    printf("\nLOOK Order : %d", head);

    for(int i = pos; i < n; i++)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    for(int i = pos - 1; i >= 0; i--)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    printf("\nTotal Head Movement = %d\n", total);
}

/* C-LOOK */
void CLOOK(int req[], int n, int head)
{
    sort(req, n);

    int total = 0, pos;

    for(pos = 0; pos < n; pos++)
        if(req[pos] > head)
            break;

    printf("\nC-LOOK Order : %d", head);

    for(int i = pos; i < n; i++)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    if(pos > 0)
    {
        total += absVal(head - req[0]);
        head = req[0];
        printf(" -> %d", head);
    }

    for(int i = 1; i < pos; i++)
    {
        total += absVal(req[i] - head);
        head = req[i];
        printf(" -> %d", head);
    }

    printf("\nTotal Head Movement = %d\n", total);
}

/* Main */
int main()
{
    int n, head, size;

    printf("Enter Number of Requests : ");
    scanf("%d", &n);

    int req[n];

    printf("Enter Requests : ");
    for(int i = 0; i < n; i++)
        scanf("%d", &req[i]);

    printf("Enter Initial Head Position : ");
    scanf("%d", &head);

    printf("Enter Disk Size : ");
    scanf("%d", &size);

    FCFS(req, n, head);
    SSTF(req, n, head);
    SCAN(req, n, head, size);
    CSCAN(req, n, head, size);
    LOOK(req, n, head);
    CLOOK(req, n, head);

    return 0;
}
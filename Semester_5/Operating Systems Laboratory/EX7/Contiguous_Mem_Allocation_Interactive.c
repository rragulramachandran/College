#include <stdio.h>

#define MAX 20

void allocateMemory(int process[], int n, int hole[], int m, int choice)
{
    int allocated[MAX], fragmentation[MAX];
    int used[MAX] = {0};

    for (int i = 0; i < n; i++)
    {
        allocated[i] = -1;
        fragmentation[i] = 0;
    }

    for (int i = 0; i < n; i++)
    {
        int index = -1;

        if (choice == 1)
        {
            for (int j = 0; j < m; j++)
            {
                if (used[j] == 0 && hole[j] >= process[i])
                {
                    index = j;
                    break;
                }
            }
        }
        else if (choice == 2)
        {
            for (int j = 0; j < m; j++)
            {
                if (used[j] == 0 && hole[j] >= process[i])
                {
                    if (index == -1 || hole[j] < hole[index])
                        index = j;
                }
            }
        }
        else if (choice == 3)
        {
            for (int j = 0; j < m; j++)
            {
                if (used[j] == 0 && hole[j] >= process[i])
                {
                    if (index == -1 || hole[j] > hole[index])
                        index = j;
                }
            }
        }

        if (index != -1)
        {
            allocated[i] = index;
            fragmentation[i] = hole[index] - process[i];
            used[index] = 1;
        }
    }

    if (choice == 1)
        printf("\n================ FIRST FIT ================\n");
    else if (choice == 2)
        printf("\n================ BEST FIT =================\n");
    else
        printf("\n================ WORST FIT ================\n");

    printf("%-10s %-20s %-18s %-27s %-10s\n",
           "Process", "Partition", "Allocation",
           "Internal Fragmentation", "Request");

    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++)
    {
        printf("P%-9d", i + 1);

        if (allocated[i] != -1)
        {
            /* Partition displayed as H1(100 MB), H2(170 MB), etc. */
            printf("H%d(%d MB)%-10s", allocated[i] + 1, hole[allocated[i]], "");
            printf("%-18s", "Allocated");
            printf("%-27d", fragmentation[i]);
            printf("%-10d\n", process[i]);
        }
        else
        {
            printf("%-20s", "-");
            printf("%-18s", "Not Allocated");
            printf("%-27s", "-");
            printf("%-10d\n", process[i]);
        }
    }
}

int main()
{
    int process[MAX], hole[MAX];
    int n, m;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter size of each process:\n");
    for (int i = 0; i < n; i++)
    {
        printf("P%d: ", i + 1);
        scanf("%d", &process[i]);
    }

    printf("\nEnter number of memory partitions: ");
    scanf("%d", &m);

    printf("Enter size of each memory partition:\n");
    for (int i = 0; i < m; i++)
    {
        printf("H%d: ", i + 1);
        scanf("%d", &hole[i]);
    }

    allocateMemory(process, n, hole, m, 1);
    allocateMemory(process, n, hole, m, 2);
    allocateMemory(process, n, hole, m, 3);

    return 0;
}

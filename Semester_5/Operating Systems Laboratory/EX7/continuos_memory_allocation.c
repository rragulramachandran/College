#include <stdio.h>

#define MAX 20

int main()
{
    int process[MAX], hole[MAX];
    int allocated[MAX], used[MAX] = {0};

    int n, m, choice;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter size of each process:\n");
    for (int i = 0; i < n; i++)
    {
        printf("P%d: ", i + 1);
        scanf("%d", &process[i]);
        allocated[i] = -1;
    }

    printf("\nEnter number of memory holes: ");
    scanf("%d", &m);

    printf("Enter size of each memory hole:\n");
    for (int i = 0; i < m; i++)
    {
        printf("H%d: ", i + 1);
        scanf("%d", &hole[i]);
    }

    printf("\nMemory Allocation Methods\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    // Allocate memory for every process
    for (int i = 0; i < n; i++)
    {
        int index = -1;

        // ---------------- FIRST FIT ----------------
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

        // ---------------- BEST FIT ----------------
        else if (choice == 2)
        {
            for (int j = 0; j < m; j++)
            {
                if (used[j] == 0 && hole[j] >= process[i])
                {
                    if (index == -1 || hole[j] < hole[index])
                    {
                        index = j;
                    }
                }
            }
        }

        // ---------------- WORST FIT ----------------
        else if (choice == 3)
        {
            for (int j = 0; j < m; j++)
            {
                if (used[j] == 0 && hole[j] >= process[i])
                {
                    if (index == -1 || hole[j] > hole[index])
                    {
                        index = j;
                    }
                }
            }
        }

        else
        {
            printf("Invalid choice!\n");
            return 0;
        }

        // Hole found
        if (index != -1)
        {
            allocated[i] = index;
            used[index] = 1;
        }
    }

    // Display result
    printf("\nProcess\tSize\tHole\n");

    for (int i = 0; i < n; i++)
    {
        printf("P%d\t%d\t", i + 1, process[i]);

        if (allocated[i] != -1)
            printf("H%d\n", allocated[i] + 1);
        else
            printf("Not Allocated\n");
    }

    return 0;
}
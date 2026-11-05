#include <stdio.h>
#define MAX 20

int main()
{
    int process[MAX], block[MAX], backup_block[MAX];
    int allocated[MAX];
    // Dynamic tracking array to snapshot the block size at the exact moment of allocation
    int remaining_at_alloc[MAX];
    int n, m, choice;
    int i, j;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter size of each process:\n");
    for (i = 0; i < n; i++)
    {
        printf("P%d: ", i + 1);
        scanf("%d", &process[i]);
    }

    printf("\nEnter number of memory blocks: ");
    scanf("%d", &m);

    printf("Enter size of each memory block:\n");
    for (i = 0; i < m; i++)
    {
        printf("B%d: ", i + 1);
        scanf("%d", &block[i]);
        backup_block[i] = block[i];
    }

    while (1)
    {
        for (i = 0; i < n; i++)
        {
            allocated[i] = -1;
            remaining_at_alloc[i] = -1;
        }

        for (i = 0; i < m; i++)
        {
            block[i] = backup_block[i];
        }

        printf("\n=============================\n");
        printf("  Memory Allocation Methods\n");
        printf("=============================\n");
        printf("1. First Fit\n");
        printf("2. Best Fit\n");
        printf("3. Worst Fit\n");
        printf("4. Exit Program\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 4)
        {
            printf("Exiting Program... Thank you!\n");
            break;
        }
        else if (choice < 1 || choice > 4)
        {
            printf("Invalid choice! Try again.\n");
            continue;
        }

        for (i = 0; i < n; i++)
        {
            int index = -1;

            // ---------------- FIRST FIT ----------------
            if (choice == 1)
            {
                for (j = 0; j < m; j++)
                {
                    if (block[j] >= process[i])
                    {
                        index = j;
                        break;
                    }
                }
            }

            // ---------------- BEST FIT (two-pass, no sorting) ----------------
            else if (choice == 2)
            {
                int best_size = -1;

                // Pass 1: find the smallest block size that can still fit the process
                for (j = 0; j < m; j++)
                {
                    if (block[j] >= process[i])
                    {
                        if (best_size == -1 || block[j] < best_size)
                        {
                            best_size = block[j];
                        }
                    }
                }

                // Pass 2: find the first block that matches that best size
                if (best_size != -1)
                {
                    for (j = 0; j < m; j++)
                    {
                        if (block[j] == best_size)
                        {
                            index = j;
                            break;
                        }
                    }
                }
            }

            // ---------------- WORST FIT (two-pass, no sorting) ----------------
            else if (choice == 3)
            {
                int worst_size = -1;

                // Pass 1: find the largest block size available
                for (j = 0; j < m; j++)
                {
                    if (block[j] >= process[i])
                    {
                        if (block[j] > worst_size)
                        {
                            worst_size = block[j];
                        }
                    }
                }

                // Pass 2: find the first block that matches that worst size
                if (worst_size != -1)
                {
                    for (j = 0; j < m; j++)
                    {
                        if (block[j] == worst_size)
                        {
                            index = j;
                            break;
                        }
                    }
                }
            }

            if (index != -1)
            {
                allocated[i] = index;
                block[index] -= process[i];
                // Store the exact snapshot of remaining space right after this process takes it
                remaining_at_alloc[i] = block[index];
            }
        }

        // Properly formatted output table showing everything in columns
        printf("\nProcess\tSize\tAllocated Block\tRemaining Block Size\n");
        for (i = 0; i < n; i++)
        {
            printf("P%d\t%d\t", i + 1, process[i]);
            if (allocated[i] != -1)
            {
                printf("B%d\t\t%d\n", allocated[i] + 1, remaining_at_alloc[i]);
            }
            else
            {
                printf("Not Allocated\t-\n");
            }
        }

        printf("\n--- Final Status of All Blocks ---\n");
        for (j = 0; j < m; j++)
        {
            printf("B%d: %d\n", j + 1, block[j]);
        }
    }

    return 0;
}
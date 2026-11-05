#include <stdio.h>

#define MAX 10

int n, m;
int available[MAX];
int max[MAX][MAX];
int allocation[MAX][MAX];
int need[MAX][MAX];

// Function to check whether system is in safe state
int isSafe()
{
    int work[MAX];
    int finish[MAX] = {0};
    int safeSequence[MAX];
    int count = 0;

    // Work = Available
    for (int j = 0; j < m; j++)
        work[j] = available[j];

    while (count < n)
    {
        int found = 0;

        for (int i = 0; i < n; i++)
        {
            if (finish[i] == 0)
            {
                int possible = 1;

                // Check Need[i] <= Work
                for (int j = 0; j < m; j++)
                {
                    if (need[i][j] > work[j])
                    {
                        possible = 0;
                        break;
                    }
                }

                if (possible)
                {
                    // Process completes and releases resources
                    for (int j = 0; j < m; j++)
                        work[j] += allocation[i][j];

                    safeSequence[count] = i;
                    count++;

                    finish[i] = 1;
                    found = 1;
                }
            }
        }

        // No process can execute
        if (found == 0)
        {
            printf("\nSystem is in UNSAFE state.\n");
            return 0;
        }
    }

    printf("\nSystem is in SAFE state.\n");

    printf("Safe Sequence: ");
    for (int i = 0; i < n; i++)
    {
        printf("P%d", safeSequence[i]);

        if (i != n - 1)
            printf(" -> ");
    }

    printf("\n");
 
    return 1;
}

int main()
{
    int process;
    int request[MAX];

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter number of resource types: ");
    scanf("%d", &m);

    // Available
    printf("\nEnter Available resources:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &available[j]);

    // Maximum matrix
    printf("\nEnter Maximum matrix:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            scanf("%d", &max[i][j]);
        }
    }

    // Allocation matrix
    printf("\nEnter Allocation matrix:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            scanf("%d", &allocation[i][j]);
        }
    }

    // Calculate Need = Max - Allocation
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    // Display Need matrix
    printf("\nNeed Matrix:\n");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }

    // Check initial safety
    if (!isSafe())
        return 0;

    // Resource Request Algorithm
    printf("\nEnter process number making request: ");
    scanf("%d", &process);

    printf("Enter request vector:\n");

    for (int j = 0; j < m; j++)
        scanf("%d", &request[j]);

    // Step 1: Request <= Need
    for (int j = 0; j < m; j++)
    {
        if (request[j] > need[process][j])
        {
            printf("\nError: Process has exceeded its maximum claim.\n");
            return 0;
        }
    }

    // Step 2: Request <= Available
    for (int j = 0; j < m; j++)
    {
        if (request[j] > available[j])
        {
            printf("\nResources are not available. Process P%d must wait.\n",
                   process);
            return 0;
        }
    }

    // Step 3: Pretend to allocate resources
    for (int j = 0; j < m; j++)
    {
        available[j] -= request[j];
        allocation[process][j] += request[j];
        need[process][j] -= request[j];
    }

    // Check safety after temporary allocation
    if (isSafe())
    {
        printf("\nRequest can be GRANTED to P%d.\n", process);
    }
    else
    {
        printf("\nRequest cannot be granted. P%d must WAIT.\n", process);

        // Restore old state
        for (int j = 0; j < m; j++)
        {
            available[j] += request[j];
            allocation[process][j] -= request[j];
            need[process][j] += request[j];
        }
    }

    return 0;
}
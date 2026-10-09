#include <stdio.h>

#define MAX 10

int n, m;
int available[MAX];
int max[MAX][MAX];
int allocation[MAX][MAX];
int need[MAX][MAX];

/* Safety algorithm: returns 1 if the system is safe, 0 otherwise */
int isSafe()
{
    int work[MAX];
    int finish[MAX] = {0};
    int safeSequence[MAX];
    int count = 0;

    /* Step 1: Work = Available */
    for (int j = 0; j < m; j++)
        work[j] = available[j];

    /* Step 2: repeat until every process has finished */
    while (count < n)
    {
        int before = count; /* processes finished before this pass */

        for (int i = 0; i < n; i++)
        {
            if (finish[i])
                continue;

            /* Check Need[i] <= Work */
            int j;
            for (j = 0; j < m; j++)
                if (need[i][j] > work[j])
                    break;

            if (j == m) /* loop completed: Need[i] <= Work */
            {
                /* Process finishes and releases its resources */
                for (int k = 0; k < m; k++)
                    work[k] += allocation[i][k];

                finish[i] = 1;
                safeSequence[count++] = i;
            }
        }

        /* Nobody could finish in this pass, so no safe sequence exists */
        if (count == before)
        {
            printf("\nSystem is in UNSAFE state.\n");
            return 0;
        }
    }

    printf("\nSystem is in SAFE state.\nSafe Sequence: ");
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

    if (n <= 0 || n > MAX || m <= 0 || m > MAX)
    {
        printf("Processes and resource types must be between 1 and %d.\n", MAX);
        return 0;
    }

    /* Available vector */
    printf("\nEnter Available resources:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &available[j]);

    /* Maximum matrix */
    printf("\nEnter Maximum matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &max[i][j]);

    /* Allocation matrix */
    printf("\nEnter Allocation matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &allocation[i][j]);

    /* Need = Max - Allocation */
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = max[i][j] - allocation[i][j];

    /* Display Need matrix */
    printf("\nNeed Matrix:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            printf("%d ", need[i][j]);
        printf("\n");
    }

    /* Check initial safety */
    if (!isSafe())
        return 0;

    /* Resource-Request algorithm */
    printf("\nEnter process number making request: ");
    scanf("%d", &process);

    if (process < 0 || process >= n)
    {
        printf("Invalid process number.\n");
        return 0;
    }

    printf("Enter request vector:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &request[j]);

    /* Step 1: Request <= Need */
    for (int j = 0; j < m; j++)
    {
        if (request[j] > need[process][j])
        {
            printf("\nError: Process has exceeded its maximum claim.\n");
            return 0;
        }
    }

    /* Step 2: Request <= Available */
    for (int j = 0; j < m; j++)
    {
        if (request[j] > available[j])
        {
            printf("\nResources are not available. P%d must wait.\n", process);
            return 0;
        }
    }

    /* Step 3: Pretend to allocate */
    for (int j = 0; j < m; j++)
    {
        available[j] -= request[j];
        allocation[process][j] += request[j];
        need[process][j] -= request[j];
    }

    /* Check safety after the pretend allocation */
    if (isSafe())
    {
        printf("\nRequest can be GRANTED to P%d.\n", process);
    }
    else
    {
        printf("\nRequest cannot be granted. P%d must WAIT.\n", process);

        /* Roll back to the old state */
        for (int j = 0; j < m; j++)
        {
            available[j] += request[j];
            allocation[process][j] -= request[j];
            need[process][j] += request[j];
        }
    }

    return 0;
}
#include <stdio.h>

int m, n;
int available[10];
int max[10][10];
int need[10][10];
int allocation[10][10];

int isSafe()
{
    int work[10];
    int finish[10] = {0};
    int safeSequence[10];

    for (int i = 0; i < m; i++)
    {
        work[i] = available[i];
    }

    int count = 0;

    while (count < n)
    {
        int before = count;

        for (int i = 0; i < n; i++)
        {
            if (finish[i])
            {
                break;
            }

            int j;
            for (j = 0; j < m; j++)
            {
                if (need[i][j] > work[j])
                {
                    break;
                }
            }

            if (j == m)
            {
                for (int k = 0; k < m; k++)
                {
                    work[k] += allocation[i][k];
                }

                finish[i] = 1;
                safeSequence[count++] = i;
            }
        }

        if (before == count)
        {
            return 0;
        }
    }

    return 1;
}

int main()
{
    printf("Enter No. Of  Process : ");
    scanf("%d", &n);

    printf("Enter No. Of  Resource Types : ");
    scanf("%d", &m);

    printf("Enter Available : ");
    for (int i = 0; i < m; i++)
    {
        scanf("%d", &available[i]);
    }

    printf("Enter Max : ");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            scanf("%d", &max[i][j]);
        }
    }

    printf("Enter Allocation : ");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            scanf("%d", &allocation[i][j]);
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    if (isSafe())
    {
        printf("Safe");
    }
    else
    {
        printf("Unsafe");
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

int absDiff(int num1, int num2)
{
    if (num1 < num2)
    {
        return num2 - num1;
    }

    return num1 - num2;
}

int *sortedCopy(int *cylst, int noOfReq)
{
    int *s = (int *)calloc(noOfReq, sizeof(int));
    for (int i = 0; i < noOfReq; i++)
    {
        s[i] = cylst[i];
    }

    for (int i = 0; i < noOfReq - 1; i++)
    {
        for (int j = i + 1; j < noOfReq; j++)
        {
            if (s[i] > s[j])
            {
                int temp = s[i];
                s[i] = s[j];
                s[j] = temp;
            }
        }
    }
    return s;
}

void moveHead(int *pos, int next, int *total)
{
    int distance = absDiff(*pos, next);
    *total += distance;

    printf("%-14d%-14d%-d\n", *pos, next, *total);

    *pos = next;
}

int FCFS(int *cylst, int noOfReq, int totalCyl, int initialheadPos)
{
    int total = 0;
    int pos = initialheadPos;

    for (int i = 0; i < noOfReq; i++)
    {
        moveHead(&pos, cylst[i], &total);
    }

    return total;
}

int SSTF(int *cylst, int noOfReq, int totalCyl, int initiaheadPos)
{
    int *done = (int *)calloc(noOfReq, sizeof(int));
    int total = 0;
    int pos = initiaheadPos;

    for (int i = 0; i < noOfReq; i++)
    {
        int best = -1;
        int bestDist = 0;

        for (int j = 0; j < noOfReq; j++)
        {
            if (!done[j])
            {
                int d = absDiff(pos, cylst[j]);

                if (best == -1 || d < bestDist)
                {
                    best = j;
                    bestDist = d;
                }
            }
        }
        done[best] = 1;
        moveHead(&pos, cylst[best], &total);
    }

    free(done);
    return total;
}

int SCAN(int *cylst, int noOfReq, int totalCyl, int initialheadPos)
{
    int direction, choice;
    printf("Choose Direction:\n");
    printf("1. Left to Right\n");
    printf("2. Right to Left\n");
    printf("Enter Choice : ");
    scanf("%d", &choice);

    printf("%-14s%-14s%s\n", "Current Pos", "Next Jump", "Total");

    direction = (choice == 1) ? 1 : 0;

    int *s = sortedCopy(cylst, noOfReq);
    int total = 0;
    int pos = initialheadPos;
    int left = 0, right;

    while (left < noOfReq && s[left] < initialheadPos)
    {
        left++;
    }
    right = left;
    while (right < noOfReq && s[right] == initialheadPos)
    {
        moveHead(&pos, s[right], &total);
        right++;
    }

    if (direction == 1)
    {
        for (int i = right; i < noOfReq; i++)
        {
            moveHead(&pos, s[i], &total);
        }
        moveHead(&pos, totalCyl - 1, &total);
        for (int i = left - 1; i >= 0; i--)
        {
            moveHead(&pos, s[i], &total);
        }
    }
    else
    {
        for (int i = left - 1; i >= 0; i--)
        {
            moveHead(&pos, s[i], &total);
        }
        moveHead(&pos, 0, &total);
        for (int i = right; i < noOfReq; i++)
        {
            moveHead(&pos, s[i], &total);
        }
    }
    free(s);
    return total;
}

int CSCAN(int *cylst, int noOfReq, int totalCyl, int initialheadPos)
{
    int direction, choice;
    printf("Choose Direction:\n");
    printf("1. Left to Right\n");
    printf("2. Right to Left\n");
    printf("Enter Choice : ");
    scanf("%d", &choice);

    printf("%-14s%-14s%s\n", "Current Pos", "Next Jump", "Total");

    direction = (choice == 1) ? 1 : 0;

    int *s = sortedCopy(cylst, noOfReq);
    int total = 0;
    int pos = initialheadPos;
    int left = 0, right;

    while (left < noOfReq && s[left] < initialheadPos)
    {
        left++;
    }
    right = left;
    while (right < noOfReq && s[right] == initialheadPos)
    {
        moveHead(&pos, s[right], &total);
        right++;
    }

    if (direction == 1)
    {
        for (int i = right; i < noOfReq; i++)
        {
            moveHead(&pos, s[i], &total);
        }
        moveHead(&pos, totalCyl - 1, &total);
        moveHead(&pos, 0, &total);
        for (int i = 0; i < left; i++)
        {
            moveHead(&pos, s[i], &total);
        }
    }
    else
    {
        for (int i = left - 1; i >= 0; i--)
        {
            moveHead(&pos, s[i], &total);
        }
        moveHead(&pos, 0, &total);
        moveHead(&pos, totalCyl - 1, &total);
        for (int i = noOfReq - 1; i >= right; i--)
        {
            moveHead(&pos, s[i], &total);
        }
    }
    free(s);
    return total;
}

int LOOK(int *cylst, int noOfReq, int totalCyl, int initialheadPos)
{
    int direction, choice;
    printf("Choose Direction:\n");
    printf("1. Left to Right\n");
    printf("2. Right to Left\n");
    printf("Enter Choice : ");
    scanf("%d", &choice);

    printf("%-14s%-14s%s\n", "Current Pos", "Next Jump", "Total");

    direction = (choice == 1) ? 1 : 0;

    int *s = sortedCopy(cylst, noOfReq);
    int total = 0;
    int pos = initialheadPos;
    int left = 0, right;

    while (left < noOfReq && s[left] < initialheadPos)
    {
        left++;
    }
    right = left;
    while (right < noOfReq && s[right] == initialheadPos)
    {
        moveHead(&pos, s[right], &total);
        right++;
    }

    if (direction == 1)
    {
        for (int i = right; i < noOfReq; i++)
        {
            moveHead(&pos, s[i], &total);
        }
        for (int i = left - 1; i >= 0; i--)
        {
            moveHead(&pos, s[i], &total);
        }
    }
    else
    {
        for (int i = left - 1; i >= 0; i--)
        {
            moveHead(&pos, s[i], &total);
        }
        for (int i = right; i < noOfReq; i++)
        {
            moveHead(&pos, s[i], &total);
        }
    }
    free(s);
    return total;
}

int CLOOK(int *cylst, int noOfReq, int totalCyl, int initialheadPos)
{
    int direction, choice;
    printf("Choose Direction:\n");
    printf("1. Left to Right\n");
    printf("2. Right to Left\n");
    printf("Enter Choice : ");
    scanf("%d", &choice);

    printf("%-14s%-14s%s\n", "Current Pos", "Next Jump", "Total");

    direction = (choice == 1) ? 1 : 0;

    int *s = sortedCopy(cylst, noOfReq);
    int total = 0;
    int pos = initialheadPos;
    int left = 0, right;

    while (left < noOfReq && s[left] < initialheadPos)
    {
        left++;
    }
    right = left;
    while (right < noOfReq && s[right] == initialheadPos)
    {
        moveHead(&pos, s[right], &total);
        right++;
    }

    if (direction == 1)
    {
        for (int i = right; i < noOfReq; i++)
        {
            moveHead(&pos, s[i], &total);
        }
        for (int i = 0; i < left; i++)
        {
            moveHead(&pos, s[i], &total);
        }
    }
    else
    {
        for (int i = left - 1; i >= 0; i--)
        {
            moveHead(&pos, s[i], &total);
        }
        for (int i = noOfReq - 1; i >= right; i--)
        {
            moveHead(&pos, s[i], &total);
        }
    }
    free(s);
    return total;
}
int main()
{
    int totalCyl, noOfReq, initialheadPos;
    // int cylst[noOfReq];

    totalCyl = 200, noOfReq = 5, initialheadPos = 53;
    int cylst[] = {10, 64, 23, 89, 167};

    /*printf("Enter No. Of Request : ");
    scanf("%d", &noOfReq);

    printf("Enter No. Of Cylinders : ");
    scanf("%d", &totalCyl);

    printf("Enter Initial head Position : ");
    scanf("%d", &initialheadPos);

    for(int i = 0; i < noOfReq; i++)
    {
        scanf("%d", &cylst[i]);
    }*/

    printf("First Come First Serve : \n");
    printf("%-14s%-14s%s\n", "Current Pos", "Next Jump", "Total");
    int FCFSTotal = FCFS(cylst, noOfReq, totalCyl, initialheadPos);

    printf("Total Head Movement : %d\n\n", FCFSTotal);

    printf("Shortest Seek Time First : \n");
    printf("%-14s%-14s%s\n", "Current Pos", "Next Jump", "Total");
    int SSTFTotal = SSTF(cylst, noOfReq, totalCyl, initialheadPos);

    printf("Total Head Movement : %d\n", SSTFTotal);

    printf("SCAN : \n");
    int SCANTotal = SCAN(cylst, noOfReq, totalCyl, initialheadPos);

    printf("Total Head Movement : %d\n", SCANTotal);

    printf("Circular-SCAN : \n");
    int CSCANTotal = CSCAN(cylst, noOfReq, totalCyl, initialheadPos);

    printf("Total Head Movement : %d\n", CSCANTotal);

    printf("\nLOOK : \n");
    int LOOKTotal = LOOK(cylst, noOfReq, totalCyl, initialheadPos);
    printf("Total Head Movement : %d\n", LOOKTotal);

    printf("\nCircular-LOOK : \n");
    int CLOOKTotal = CLOOK(cylst, noOfReq, totalCyl, initialheadPos);
    printf("Total Head Movement : %d\n", CLOOKTotal);

    return 0;
}

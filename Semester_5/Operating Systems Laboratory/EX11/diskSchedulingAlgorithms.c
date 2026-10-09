#include <stdio.h>
#include <stdlib.h>

/* Algorithm prototypes */
int FCFS(int *cylst, int noOfReq, int totalCyl, int initialHeadPos);
int SSTF(int *cylst, int noOfReq, int totalCyl, int initialHeadPos);
int SCAN(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction);
int CSCAN(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction);
int LOOK(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction);
int CLOOK(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction);

/* ---------- Helper: absolute difference ---------- */
int absDiff(int a, int b) { return (a > b) ? (a - b) : (b - a); }

/* ---------- Helper: comparator for qsort ---------- */
int cmp(const void *a, const void *b)
{
    return (*(const int *)a - *(const int *)b);
}

/* ---------- Helper: make a sorted copy of the request list ---------- */
int *sortedCopy(int *cylst, int n)
{
    int *s = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        s[i] = cylst[i];
    qsort(s, n, sizeof(int), cmp);
    return s;
}

/* Every move uses the same table format and cumulative total. */
void printTable(const char *name)
{
    printf("\n================ %s ================\n", name);
    printf("%-18s %-18s %-14s %-20s %s\n",
           "Current position", "Jump to", "Head movement", "Total head movement", "Action");
}

void moveHead(int *pos, int next, int *total, const char *action)
{
    int distance = absDiff(*pos, next);
    *total += distance;
    printf("%-18d %-18d %-14d %-20d %s\n", *pos, next, distance, *total, action);
    *pos = next;
}

int readDirection(const char *name)
{
    int direction;
    printf("\n%s initial direction:\n", name);
    printf("1. Left to right (toward higher cylinders)\n");
    printf("2. Right to left (toward lower cylinders)\n");
    printf("Enter direction (1 or 2): ");
    if (scanf("%d", &direction) != 1 || (direction != 1 && direction != 2))
    {
        printf("Invalid direction. Enter 1 or 2.\n");
        exit(EXIT_FAILURE);
    }
    return direction;
}

/* ============================ FCFS ============================ */
int FCFS(int *cylst, int noOfReq, int totalCyl, int initialHeadPos)
{
    (void)totalCyl;
    int total = 0;
    int pos = initialHeadPos;

    for (int i = 0; i < noOfReq; i++)
    {
        moveHead(&pos, cylst[i], &total, "Request");
    }
    return total;
}

/* ============================ SSTF ============================ */
int SSTF(int *cylst, int noOfReq, int totalCyl, int initialHeadPos)
{
    int *done =
        (int *)calloc(noOfReq, sizeof(int)); /* 0 = pending, 1 = served */
    (void)totalCyl;
    int total = 0;
    int pos = initialHeadPos;

    for (int count = 0; count < noOfReq; count++)
    {
        int best = -1;
        int bestDist = 0;

        /* find the closest pending request */
        for (int i = 0; i < noOfReq; i++)
        {
            if (!done[i])
            {
                int d = absDiff(pos, cylst[i]);
                if (best == -1 || d < bestDist)
                {
                    best = i;
                    bestDist = d;
                }
            }
        }
        done[best] = 1;
        moveHead(&pos, cylst[best], &total, "Request");
    }

    free(done);
    return total;
}

/* ============================ SCAN ============================ */
int SCAN(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction)
{
    int *s = sortedCopy(cylst, noOfReq);
    int total = 0;
    int pos = initialHeadPos;
    int left = 0;
    int right;

    /* Serve requests at the initial position before moving either way. */
    while (left < noOfReq && s[left] < initialHeadPos)
        left++;
    right = left;
    while (right < noOfReq && s[right] == initialHeadPos)
        moveHead(&pos, s[right++], &total, "Request");

    if (direction == 1)
    {
        for (int i = right; i < noOfReq; i++)
            moveHead(&pos, s[i], &total, "Request");
        if ((right < noOfReq || left > 0) && pos != totalCyl - 1)
            moveHead(&pos, totalCyl - 1, &total, "Disk end");
        for (int i = left - 1; i >= 0; i--)
            moveHead(&pos, s[i], &total, "Request");
    }
    else
    {
        for (int i = left - 1; i >= 0; i--)
            moveHead(&pos, s[i], &total, "Request");
        if ((left > 0 || right < noOfReq) && pos != 0)
            moveHead(&pos, 0, &total, "Disk end");
        for (int i = right; i < noOfReq; i++)
            moveHead(&pos, s[i], &total, "Request");
    }

    free(s);
    return total;
}

/* ============================ CSCAN ============================ */
int CSCAN(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction)
{
    int *s = sortedCopy(cylst, noOfReq);
    int total = 0;
    int pos = initialHeadPos;
    int left = 0;
    int right;

    /* Serve requests at the initial position before moving either way. */
    while (left < noOfReq && s[left] < initialHeadPos)
        left++;
    right = left;
    while (right < noOfReq && s[right] == initialHeadPos)
        moveHead(&pos, s[right++], &total, "Request");

    if (direction == 1)
    {
        for (int i = right; i < noOfReq; i++)
            moveHead(&pos, s[i], &total, "Request");
        if ((right < noOfReq || left > 0) && pos != totalCyl - 1)
            moveHead(&pos, totalCyl - 1, &total, "Disk end");
        if (left > 0)
            moveHead(&pos, 0, &total, "Circular jump");
        for (int i = 0; i < left; i++)
            moveHead(&pos, s[i], &total, "Request");
    }
    else
    {
        for (int i = left - 1; i >= 0; i--)
            moveHead(&pos, s[i], &total, "Request");
        if ((left > 0 || right < noOfReq) && pos != 0)
            moveHead(&pos, 0, &total, "Disk end");
        if (right < noOfReq)
            moveHead(&pos, totalCyl - 1, &total, "Circular jump");
        for (int i = noOfReq - 1; i >= right; i--)
            moveHead(&pos, s[i], &total, "Request");
    }

    free(s);
    return total;
}

/* ============================ LOOK ============================ */
int LOOK(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction)
{
    int *s = sortedCopy(cylst, noOfReq);
    int total = 0;
    int pos = initialHeadPos;
    int left = 0;
    int right;
    (void)totalCyl;

    /* Serve requests at the initial position before moving either way. */
    while (left < noOfReq && s[left] < initialHeadPos)
        left++;
    right = left;
    while (right < noOfReq && s[right] == initialHeadPos)
        moveHead(&pos, s[right++], &total, "Request");

    if (direction == 1)
    {
        for (int i = right; i < noOfReq; i++)
            moveHead(&pos, s[i], &total, "Request");
        for (int i = left - 1; i >= 0; i--)
            moveHead(&pos, s[i], &total, "Request");
    }
    else
    {
        for (int i = left - 1; i >= 0; i--)
            moveHead(&pos, s[i], &total, "Request");
        for (int i = right; i < noOfReq; i++)
            moveHead(&pos, s[i], &total, "Request");
    }

    free(s);
    return total;
}

/* ============================ CLOOK ============================ */
int CLOOK(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction)
{
    int *s = sortedCopy(cylst, noOfReq);
    int total = 0;
    int pos = initialHeadPos;
    int left = 0;
    int right;
    (void)totalCyl;

    /* Serve requests at the initial position before moving either way. */
    while (left < noOfReq && s[left] < initialHeadPos)
        left++;
    right = left;
    while (right < noOfReq && s[right] == initialHeadPos)
        moveHead(&pos, s[right++], &total, "Request");

    if (direction == 1)
    {
        for (int i = right; i < noOfReq; i++)
            moveHead(&pos, s[i], &total, "Request");
        for (int i = 0; i < left; i++)
            moveHead(&pos, s[i], &total, i == 0 ? "Circular jump / request" : "Request");
    }
    else
    {
        for (int i = left - 1; i >= 0; i--)
            moveHead(&pos, s[i], &total, "Request");
        for (int i = noOfReq - 1; i >= right; i--)
            moveHead(&pos, s[i], &total, i == noOfReq - 1 ? "Circular jump / request" : "Request");
    }

    free(s);
    return total;
}

/* ============================ MAIN ============================ */
int main(void)
{
    int noOfReq, totalCyl, initialHeadPos;

    printf("Enter number of requests: ");
    scanf("%d", &noOfReq);

    printf("Enter total number of cylinders: ");
    scanf("%d", &totalCyl);

    printf("Enter initial head position: ");
    scanf("%d", &initialHeadPos);

    if (noOfReq <= 0 || totalCyl <= 0 || initialHeadPos < 0 ||
        initialHeadPos >= totalCyl)
    {
        printf("Invalid input.\n");
        return 1;
    }

    int *cylst = (int *)malloc(noOfReq * sizeof(int));
    printf("Enter the cylinder request queue: ");
    for (int i = 0; i < noOfReq; i++)
    {
        scanf("%d", &cylst[i]);
        if (cylst[i] < 0 || cylst[i] >= totalCyl)
        {
            printf("Request %d is outside the disk range.\n", cylst[i]);
            free(cylst);
            return 1;
        }
    }

    int total, direction;
    printTable("FCFS");
    total = FCFS(cylst, noOfReq, totalCyl, initialHeadPos);
    printf("Total head movement: %d\n", total);

    printTable("SSTF");
    total = SSTF(cylst, noOfReq, totalCyl, initialHeadPos);
    printf("Total head movement: %d\n", total);

    direction = readDirection("SCAN");
    printTable("SCAN");
    total = SCAN(cylst, noOfReq, totalCyl, initialHeadPos, direction);
    printf("Total head movement: %d\n", total);

    direction = readDirection("C-SCAN");
    printTable("C-SCAN");
    total = CSCAN(cylst, noOfReq, totalCyl, initialHeadPos, direction);
    printf("Total head movement: %d\n", total);

    direction = readDirection("LOOK");
    printTable("LOOK");
    total = LOOK(cylst, noOfReq, totalCyl, initialHeadPos, direction);
    printf("Total head movement: %d\n", total);

    direction = readDirection("C-LOOK");
    printTable("C-LOOK");
    total = CLOOK(cylst, noOfReq, totalCyl, initialHeadPos, direction);
    printf("Total head movement: %d\n", total);

    free(cylst);
    return 0;
}
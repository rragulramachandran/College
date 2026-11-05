#include <stdio.h>
#include <stdlib.h>

#define MAX 100

/* Function prototypes */
int FCFS(int *cylst, int reqcnt, int totcy, int initheadpos);
int SSTF(int *cylst, int reqcnt, int totcy, int initheadpos);
int SCAN(int *cylst, int reqcnt, int totcy, int initheadpos);
int CSCAN(int *cylst, int reqcnt, int totcy, int initheadpos);
int LOOK(int *cylst, int reqcnt, int totcy, int initheadpos);
int CLOOK(int *cylst, int reqcnt, int totcy, int initheadpos);

void sort(int a[], int n);

/* -------------------------------------------------- */
/* FCFS */
/* -------------------------------------------------- */
int FCFS(int *cylst, int reqcnt, int totcy, int initheadpos)
{
    int i, head = initheadpos, movement = 0;

    printf("\nFCFS Path: %d", head);

    for (i = 0; i < reqcnt; i++)
    {
        movement += abs(head - cylst[i]);
        head = cylst[i];
        printf(" -> %d", head);
    }

    return movement;
}

/* -------------------------------------------------- */
/* SSTF */
/* -------------------------------------------------- */
int SSTF(int *cylst, int reqcnt, int totcy, int initheadpos)
{
    int visited[MAX] = {0};
    int i, j, head = initheadpos;
    int movement = 0;
    int min, index, distance;

    printf("\nSSTF Path: %d", head);

    for (i = 0; i < reqcnt; i++)
    {
        min = 99999;
        index = -1;

        for (j = 0; j < reqcnt; j++)
        {
            if (visited[j] == 0)
            {
                distance = abs(head - cylst[j]);

                if (distance < min)
                {
                    min = distance;
                    index = j;
                }
            }
        }

        visited[index] = 1;
        movement += abs(head - cylst[index]);
        head = cylst[index];

        printf(" -> %d", head);
    }

    return movement;
}

/* -------------------------------------------------- */
/* SCAN */
/* -------------------------------------------------- */
int SCAN(int *cylst, int reqcnt, int totcy, int initheadpos)
{
    int a[MAX], i, head = initheadpos;
    int movement = 0;
    int pos = 0;
    int direction;

    printf("\nEnter direction for SCAN (1 = Right, 0 = Left): ");
    scanf("%d", &direction);

    for (i = 0; i < reqcnt; i++)
        a[i] = cylst[i];

    sort(a, reqcnt);

    while (pos < reqcnt && a[pos] < head)
        pos++;

    printf("\nSCAN Path: %d", head);

    if (direction == 1)
    {
        /* Move towards higher cylinders */
        for (i = pos; i < reqcnt; i++)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }

        /* Go to last cylinder */
        if (head != totcy - 1)
        {
            movement += abs(head - (totcy - 1));
            head = totcy - 1;
            printf(" -> %d", head);
        }

        /* Reverse direction */
        for (i = pos - 1; i >= 0; i--)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }
    }
    else
    {
        /* Move towards lower cylinders */
        for (i = pos - 1; i >= 0; i--)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }

        /* Go to first cylinder */
        if (head != 0)
        {
            movement += head;
            head = 0;
            printf(" -> %d", head);
        }

        /* Reverse direction */
        for (i = pos; i < reqcnt; i++)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }
    }

    return movement;
}

/* -------------------------------------------------- */
/* C-SCAN */
/* -------------------------------------------------- */
int CSCAN(int *cylst, int reqcnt, int totcy, int initheadpos)
{
    int a[MAX], i, head = initheadpos;
    int movement = 0;
    int pos = 0;
    int direction;

    printf("\nEnter direction for C-SCAN (1 = Right, 0 = Left): ");
    scanf("%d", &direction);

    for (i = 0; i < reqcnt; i++)
        a[i] = cylst[i];

    sort(a, reqcnt);

    while (pos < reqcnt && a[pos] < head)
        pos++;

    printf("\nC-SCAN Path: %d", head);

    if (direction == 1)
    {
        /* Move towards higher cylinders */
        for (i = pos; i < reqcnt; i++)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }

        /* Go to last cylinder */
        if (head != totcy - 1)
        {
            movement += (totcy - 1) - head;
            head = totcy - 1;
            printf(" -> %d", head);
        }

        /* Jump to first cylinder */
        movement += totcy - 1;
        head = 0;
        printf(" -> %d", head);

        /* Continue in same direction */
        for (i = 0; i < pos; i++)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }
    }
    else
    {
        /* Move towards lower cylinders */
        for (i = pos - 1; i >= 0; i--)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }

        /* Go to first cylinder */
        if (head != 0)
        {
            movement += head;
            head = 0;
            printf(" -> %d", head);
        }

        /* Jump to last cylinder */
        movement += totcy - 1;
        head = totcy - 1;
        printf(" -> %d", head);

        /* Continue in same direction */
        for (i = reqcnt - 1; i >= pos; i--)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }
    }

    return movement;
}

/* -------------------------------------------------- */
/* LOOK */
/* -------------------------------------------------- */
int LOOK(int *cylst, int reqcnt, int totcy, int initheadpos)
{
    int a[MAX], i, head = initheadpos;
    int movement = 0;
    int pos = 0;
    int direction;

    printf("\nEnter direction for LOOK (1 = Right, 0 = Left): ");
    scanf("%d", &direction);

    for (i = 0; i < reqcnt; i++)
        a[i] = cylst[i];

    sort(a, reqcnt);

    while (pos < reqcnt && a[pos] < head)
        pos++;

    printf("\nLOOK Path: %d", head);

    if (direction == 1)
    {
        /* Move right */
        for (i = pos; i < reqcnt; i++)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }

        /* Reverse without going to disk end */
        for (i = pos - 1; i >= 0; i--)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }
    }
    else
    {
        /* Move left */
        for (i = pos - 1; i >= 0; i--)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }

        /* Reverse without going to disk end */
        for (i = pos; i < reqcnt; i++)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }
    }

    return movement;
}

/* -------------------------------------------------- */
/* C-LOOK */
/* -------------------------------------------------- */
int CLOOK(int *cylst, int reqcnt, int totcy, int initheadpos)
{
    int a[MAX], i, head = initheadpos;
    int movement = 0;
    int pos = 0;
    int direction;

    printf("\nEnter direction for C-LOOK (1 = Right, 0 = Left): ");
    scanf("%d", &direction);

    for (i = 0; i < reqcnt; i++)
        a[i] = cylst[i];

    sort(a, reqcnt);

    while (pos < reqcnt && a[pos] < head)
        pos++;

    printf("\nC-LOOK Path: %d", head);

    if (direction == 1)
    {
        /* Move right */
        for (i = pos; i < reqcnt; i++)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }

        /* Jump to lowest request */
        if (pos > 0)
        {
            movement += abs(head - a[0]);
            head = a[0];
            printf(" -> %d", head);
        }

        /* Continue right */
        for (i = 1; i < pos; i++)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }
    }
    else
    {
        /* Move left */
        for (i = pos - 1; i >= 0; i--)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }

        /* Jump to highest request */
        if (pos < reqcnt)
        {
            movement += abs(head - a[reqcnt - 1]);
            head = a[reqcnt - 1];
            printf(" -> %d", head);
        }

        /* Continue left */
        for (i = reqcnt - 2; i >= pos; i--)
        {
            movement += abs(head - a[i]);
            head = a[i];
            printf(" -> %d", head);
        }
    }

    return movement;
}

/* -------------------------------------------------- */
/* Sorting function */
/* -------------------------------------------------- */
void sort(int a[], int n)
{
    int i, j, temp;

    for (i = 0; i < n - 1; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (a[i] > a[j])
            {
                temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }
}

/* -------------------------------------------------- */
/* Main function */
/* -------------------------------------------------- */
int main()
{
    int cylst[MAX];
    int reqcnt, totcy, initheadpos;
    int i;
    int choice, result;

    printf("Enter total number of cylinders: ");
    scanf("%d", &totcy);

    printf("Enter number of requests: ");
    scanf("%d", &reqcnt);

    printf("Enter the request queue:\n");
    for (i = 0; i < reqcnt; i++)
        scanf("%d", &cylst[i]);

    printf("Enter initial head position: ");
    scanf("%d", &initheadpos);

    do
    {
        printf("\n\n===== DISK SCHEDULING =====\n");
        printf("1. FCFS\n");
        printf("2. SSTF\n");
        printf("3. SCAN\n");
        printf("4. C-SCAN\n");
        printf("5. LOOK\n");
        printf("6. C-LOOK\n");
        printf("7. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                result = FCFS(cylst, reqcnt, totcy, initheadpos);
                printf("\nTotal Head Movement = %d", result);
                break;

            case 2:
                result = SSTF(cylst, reqcnt, totcy, initheadpos);
                printf("\nTotal Head Movement = %d", result);
                break;

            case 3:
                result = SCAN(cylst, reqcnt, totcy, initheadpos);
                printf("\nTotal Head Movement = %d", result);
                break;

            case 4:
                result = CSCAN(cylst, reqcnt, totcy, initheadpos);
                printf("\nTotal Head Movement = %d", result);
                break;

            case 5:
                result = LOOK(cylst, reqcnt, totcy, initheadpos);
                printf("\nTotal Head Movement = %d", result);
                break;

            case 6:
                result = CLOOK(cylst, reqcnt, totcy, initheadpos);
                printf("\nTotal Head Movement = %d", result);
                break;

            case 7:
                printf("Program terminated.\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 7);

    return 0;
}
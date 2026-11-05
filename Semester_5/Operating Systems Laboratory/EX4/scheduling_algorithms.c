#include <stdio.h>
#include <float.h>
#include <limits.h>

#define MAX 100

int n;
int priority[MAX];

void printTable(int *processlst, float *bursttime, float *arrivaltime,
                float *completion, float *waiting, float *turnaround)
{
    printf("\n%-10s%-10s%-10s%-12s%-10s%-12s\n",
           "Process", "Arrival", "Burst", "Completion", "Waiting", "Turnaround");

    for (int i = 0; i < n; i++)
    {
        printf("P%-9d%-10.2f%-10.2f%-12.2f%-10.2f%-12.2f\n",
               processlst[i], arrivaltime[i], bursttime[i],
               completion[i], waiting[i], turnaround[i]);
    }
}

void printPriorityTable(int *processlst, float *bursttime, float *arrivaltime,
                        float *completion, float *waiting, float *turnaround)
{
    printf("\n%-10s%-10s%-10s%-10s%-12s%-10s%-12s\n",
           "Process", "Arrival", "Burst", "Priority",
           "Completion", "Waiting", "Turnaround");

    for (int i = 0; i < n; i++)
    {
        printf("P%-9d%-10.2f%-10.2f%-10d%-12.2f%-10.2f%-12.2f\n",
               processlst[i], arrivaltime[i], bursttime[i], priority[i],
               completion[i], waiting[i], turnaround[i]);
    }
}

void fcfs(int *processlst, float *bursttime, float *arrivaltime,
          float *awtime, float *atattime)
{
    int process[MAX];
    float burst[MAX], arrival[MAX];
    float completion[MAX], waiting[MAX], turnaround[MAX];

    for (int i = 0; i < n; i++)
    {
        process[i] = processlst[i];
        burst[i] = bursttime[i];
        arrival[i] = arrivaltime[i];
    }

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arrival[j] > arrival[j + 1])
            {
                float temp = arrival[j];
                arrival[j] = arrival[j + 1];
                arrival[j + 1] = temp;

                temp = burst[j];
                burst[j] = burst[j + 1];
                burst[j + 1] = temp;

                int t = process[j];
                process[j] = process[j + 1];
                process[j + 1] = t;
            }
        }
    }

    float time = 0, totalWT = 0, totalTAT = 0;

    for (int i = 0; i < n; i++)
    {
        if (time < arrival[i])
            time = arrival[i];

        time += burst[i];
        completion[i] = time;
        turnaround[i] = completion[i] - arrival[i];
        waiting[i] = turnaround[i] - burst[i];
        totalWT += waiting[i];
        totalTAT += turnaround[i];
    }

    printf("\n===== FCFS Scheduling =====\n");
    printTable(process, burst, arrival, completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

void sjfs(int *processlst, float *bursttime, float *arrivaltime,
          float *awtime, float *atattime)
{
    float completion[MAX], waiting[MAX], turnaround[MAX];
    int done[MAX] = {0};
    float time = 0, totalWT = 0, totalTAT = 0;
    int completed = 0;

    while (completed < n)
    {
        int chosen = -1;
        float minBurst = FLT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (!done[i] && arrivaltime[i] <= time && bursttime[i] < minBurst)
            {
                minBurst = bursttime[i];
                chosen = i;
            }
        }

        if (chosen == -1)
        {
            float nextArrival = FLT_MAX;
            for (int i = 0; i < n; i++)
                if (!done[i] && arrivaltime[i] < nextArrival)
                    nextArrival = arrivaltime[i];

            time = nextArrival;
            continue;
        }

        time += bursttime[chosen];
        completion[chosen] = time;
        turnaround[chosen] = completion[chosen] - arrivaltime[chosen];
        waiting[chosen] = turnaround[chosen] - bursttime[chosen];
        totalWT += waiting[chosen];
        totalTAT += turnaround[chosen];
        done[chosen] = 1;
        completed++;
    }

    printf("\n===== SJF (Non-preemptive) Scheduling =====\n");
    printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

void srtf(int *processlst, float *bursttime, float *arrivaltime,
          float *awtime, float *atattime)
{
    float remaining[MAX], completion[MAX], waiting[MAX], turnaround[MAX];

    for (int i = 0; i < n; i++)
        remaining[i] = bursttime[i];

    float time = 0, totalWT = 0, totalTAT = 0;
    int completed = 0;

    while (completed < n)
    {
        int chosen = -1;
        float minBurst = FLT_MAX, nextArrival = FLT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (remaining[i] > 0)
            {
                if (arrivaltime[i] <= time)
                {
                    if (remaining[i] < minBurst)
                    {
                        minBurst = remaining[i];
                        chosen = i;
                    }
                }
                else if (arrivaltime[i] < nextArrival)
                {
                    nextArrival = arrivaltime[i];
                }
            }
        }

        if (chosen == -1)
        {
            time = nextArrival;
            continue;
        }

        float run = remaining[chosen];
        if (nextArrival < time + run)
            run = nextArrival - time;

        time += run;
        remaining[chosen] -= run;

        if (remaining[chosen] <= 0)
        {
            completion[chosen] = time;
            turnaround[chosen] = completion[chosen] - arrivaltime[chosen];
            waiting[chosen] = turnaround[chosen] - bursttime[chosen];
            totalWT += waiting[chosen];
            totalTAT += turnaround[chosen];
            completed++;
        }
    }

    printf("\n===== SRTF (Preemptive SJF) Scheduling =====\n");
    printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

void prioritySchedule(int *processlst, float *bursttime, float *arrivaltime,
                      float *awtime, float *atattime)
{
    float completion[MAX], waiting[MAX], turnaround[MAX];
    int done[MAX] = {0};
    float time = 0, totalWT = 0, totalTAT = 0;
    int completed = 0;

    while (completed < n)
    {
        int chosen = -1, bestPriority = INT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (!done[i] && arrivaltime[i] <= time &&
                (chosen == -1 || priority[i] < bestPriority))
            {
                bestPriority = priority[i];
                chosen = i;
            }
        }

        if (chosen == -1)
        {
            float nextArrival = FLT_MAX;
            for (int i = 0; i < n; i++)
                if (!done[i] && arrivaltime[i] < nextArrival)
                    nextArrival = arrivaltime[i];

            time = nextArrival;
            continue;
        }

        time += bursttime[chosen];
        completion[chosen] = time;
        turnaround[chosen] = completion[chosen] - arrivaltime[chosen];
        waiting[chosen] = turnaround[chosen] - bursttime[chosen];
        totalWT += waiting[chosen];
        totalTAT += turnaround[chosen];
        done[chosen] = 1;
        completed++;
    }

    printf("\n===== Non-preemptive Priority Scheduling =====\n");
    printPriorityTable(processlst, bursttime, arrivaltime,
                       completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

void priorityPreemptive(int *processlst, float *bursttime, float *arrivaltime,
                        float *awtime, float *atattime)
{
    float remaining[MAX], completion[MAX], waiting[MAX], turnaround[MAX];

    for (int i = 0; i < n; i++)
        remaining[i] = bursttime[i];

    float time = 0, totalWT = 0, totalTAT = 0;
    int completed = 0, running = -1;

    while (completed < n)
    {
        int chosen = -1, bestPriority = INT_MAX;
        float nextArrival = FLT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (remaining[i] > 0)
            {
                if (arrivaltime[i] <= time)
                {
                    if (chosen == -1 || priority[i] < bestPriority ||
                        (priority[i] == bestPriority && i == running))
                    {
                        bestPriority = priority[i];
                        chosen = i;
                    }
                }
                else if (arrivaltime[i] < nextArrival)
                {
                    nextArrival = arrivaltime[i];
                }
            }
        }

        if (chosen == -1)
        {
            time = nextArrival;
            running = -1;
            continue;
        }

        running = chosen;
        float run = remaining[chosen];

        if (nextArrival < time + run)
            run = nextArrival - time;

        time += run;
        remaining[chosen] -= run;

        if (remaining[chosen] <= 0)
        {
            completion[chosen] = time;
            turnaround[chosen] = completion[chosen] - arrivaltime[chosen];
            waiting[chosen] = turnaround[chosen] - bursttime[chosen];
            totalWT += waiting[chosen];
            totalTAT += turnaround[chosen];
            running = -1;
            completed++;
        }
    }

    printf("\n===== Preemptive Priority Scheduling =====\n");
    printPriorityTable(processlst, bursttime, arrivaltime,
                       completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

void roundRobin(int *processlst, float *bursttime, float *arrivaltime,
                float *awtime, float *atattime, float timequantum)
{
    float remaining[MAX], completion[MAX], waiting[MAX], turnaround[MAX];
    int order[MAX], queue[MAX];
    int count = 0, next = 0, completed = 0;
    float time = 0, totalWT = 0, totalTAT = 0;

    for (int i = 0; i < n; i++)
    {
        remaining[i] = bursttime[i];
        order[i] = i;
    }

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arrivaltime[order[j]] > arrivaltime[order[j + 1]])
            {
                int temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }

    while (completed < n)
    {
        if (count == 0 && next < n)
        {
            if (time < arrivaltime[order[next]])
                time = arrivaltime[order[next]];

            while (next < n && arrivaltime[order[next]] <= time)
                queue[count++] = order[next++];
        }

        int i = queue[0];
        for (int j = 1; j < count; j++)
            queue[j - 1] = queue[j];
        count--;

        float slice = remaining[i] < timequantum ? remaining[i] : timequantum;
        time += slice;
        remaining[i] -= slice;

        while (next < n && arrivaltime[order[next]] <= time)
            queue[count++] = order[next++];

        if (remaining[i] > 0)
        {
            queue[count++] = i;
        }
        else
        {
            completion[i] = time;
            turnaround[i] = completion[i] - arrivaltime[i];
            waiting[i] = turnaround[i] - bursttime[i];
            totalWT += waiting[i];
            totalTAT += turnaround[i];
            completed++;
        }
    }

    printf("\n===== Round Robin (Quantum = %.2f) =====\n", timequantum);
    printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

int main(void)
{
    int processlst[MAX], choice;
    float bursttime[MAX], arrivaltime[MAX];
    float awtime, atattime, timequantum;

    printf("Enter number of processes (1 to %d): ", MAX);
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX)
    {
        printf("Invalid number of processes.\n");
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        processlst[i] = i + 1;
        printf("\nProcess P%d\n", processlst[i]);
        printf("Arrival time: ");
        if (scanf("%f", &arrivaltime[i]) != 1 || arrivaltime[i] < 0)
            return 1;
        printf("Burst time: ");
        if (scanf("%f", &bursttime[i]) != 1 || bursttime[i] <= 0)
            return 1;
        printf("Priority (smaller number = higher priority): ");
        if (scanf("%d", &priority[i]) != 1)
            return 1;
    }

    do
    {
        printf("\n===== CPU Scheduling Menu =====\n");
        printf("1. FCFS\n");
        printf("2. SJF (Non-preemptive)\n");
        printf("3. SRTF (Preemptive SJF)\n");
        printf("4. Priority (Non-preemptive)\n");
        printf("5. Priority (Preemptive)\n");
        printf("6. Round Robin\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
            return 1;

        switch (choice)
        {
            case 1:
                fcfs(processlst, bursttime, arrivaltime, &awtime, &atattime);
                break;
            case 2:
                sjfs(processlst, bursttime, arrivaltime, &awtime, &atattime);
                break;
            case 3:
                srtf(processlst, bursttime, arrivaltime, &awtime, &atattime);
                break;
            case 4:
                prioritySchedule(processlst, bursttime, arrivaltime,
                                 &awtime, &atattime);
                break;
            case 5:
                priorityPreemptive(processlst, bursttime, arrivaltime,
                                   &awtime, &atattime);
                break;
            case 6:
                printf("Enter time quantum: ");
                if (scanf("%f", &timequantum) != 1 || timequantum <= 0)
                {
                    printf("Invalid time quantum.\n");
                    continue;
                }
                roundRobin(processlst, bursttime, arrivaltime,
                           &awtime, &atattime, timequantum);
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
                continue;
        }

        if (choice >= 1 && choice <= 6)
        {
            printf("\nAverage Waiting Time: %.2f\n", awtime);
            printf("Average Turnaround Time: %.2f\n", atattime);
        }
    } while (choice != 0);

    return 0;
}

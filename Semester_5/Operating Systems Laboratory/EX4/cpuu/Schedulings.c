#include <stdio.h>

void fcfs();
void sjf();
void roundRobin();

int main()
{
    int choice;

    do
    {
        printf("\n========== CPU Scheduling ==========\n");
        printf("1. FCFS Scheduling\n");
        printf("2. SJF Scheduling\n");
        printf("3. Round Robin Scheduling\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                fcfs();
                break;

            case 2:
                sjf();
                break;

            case 3:
                roundRobin();
                break;

            case 4:
                printf("Program Ended.\n");
                break;

            default:
                printf("Invalid Choice!\n");
        }

    } while(choice != 4);

    return 0;
}


/*---------------- FCFS SCHEDULING ----------------*/

void fcfs()
{
    int n, i;
    int at[20], bt[20];
    int ct[20], wt[20], tat[20];

    int time = 0;
    float avgWT = 0, avgTAT = 0;

    printf("\nEnter number of processes: ");
    scanf("%d", &n);

    for(i = 0; i < n; i++)
    {
        printf("\nProcess P%d\n", i + 1);

        printf("Arrival Time : ");
        scanf("%d", &at[i]);

        printf("Burst Time   : ");
        scanf("%d", &bt[i]);
    }

    /* FCFS executes processes in arrival order */
    for(i = 0; i < n; i++)
    {
        if(time < at[i])
        {
            time = at[i];
        }

        time += bt[i];

        ct[i] = time;
        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];

        avgWT += wt[i];
        avgTAT += tat[i];
    }

    printf("\nProcess\tAT\tBT\tCT\tWT\tTAT\n");

    for(i = 0; i < n; i++)
    {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               i + 1, at[i], bt[i], ct[i], wt[i], tat[i]);
    }

    printf("\nAverage Waiting Time = %.2f", avgWT / n);
    printf("\nAverage Turnaround Time = %.2f\n", avgTAT / n);
}


/*---------------- SJF SCHEDULING ----------------*/

void sjf()
{
    int n, i;
    int at[20], bt[20];
    int ct[20], wt[20], tat[20];
    int completed[20] = {0};

    int time = 0, count = 0;

    float avgWT = 0, avgTAT = 0;

    printf("\nEnter number of processes: ");
    scanf("%d", &n);

    for(i = 0; i < n; i++)
    {
        printf("\nProcess P%d\n", i + 1);

        printf("Arrival Time : ");
        scanf("%d", &at[i]);

        printf("Burst Time   : ");
        scanf("%d", &bt[i]);
    }

    while(count < n)
    {
        int index = -1;
        int smallest = 999;

        /* Find process with smallest burst time */
        for(i = 0; i < n; i++)
        {
            if(at[i] <= time && completed[i] == 0 && bt[i] < smallest)
            {
                smallest = bt[i];
                index = i;
            }
        }

        /* If no process has arrived */
        if(index == -1)
        {
            time++;
        }
        else
        {
            time += bt[index];

            ct[index] = time;
            tat[index] = ct[index] - at[index];
            wt[index] = tat[index] - bt[index];

            avgWT += wt[index];
            avgTAT += tat[index];

            completed[index] = 1;
            count++;
        }
    }

    printf("\nProcess\tAT\tBT\tCT\tWT\tTAT\n");

    for(i = 0; i < n; i++)
    {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               i + 1, at[i], bt[i], ct[i], wt[i], tat[i]);
    }

    printf("\nAverage Waiting Time = %.2f", avgWT / n);
    printf("\nAverage Turnaround Time = %.2f\n", avgTAT / n);
}


/*---------------- ROUND ROBIN SCHEDULING ----------------*/

void roundRobin()
{
    int n, i;
    int at[20], bt[20], rt[20];
    int ct[20], wt[20], tat[20];

    int quantum;
    int time = 0;
    int completed = 0;

    float avgWT = 0, avgTAT = 0;

    printf("\nEnter number of processes: ");
    scanf("%d", &n);

    for(i = 0; i < n; i++)
    {
        printf("\nProcess P%d\n", i + 1);

        printf("Arrival Time : ");
        scanf("%d", &at[i]);

        printf("Burst Time   : ");
        scanf("%d", &bt[i]);

        rt[i] = bt[i];
    }

    printf("\nEnter Time Quantum: ");
    scanf("%d", &quantum);

    while(completed < n)
    {
        int executed = 0;

        for(i = 0; i < n; i++)
        {
            if(at[i] <= time && rt[i] > 0)
            {
                executed = 1;

                if(rt[i] > quantum)
                {
                    time += quantum;
                    rt[i] -= quantum;
                }
                else
                {
                    time += rt[i];
                    rt[i] = 0;

                    completed++;

                    ct[i] = time;
                    tat[i] = ct[i] - at[i];
                    wt[i] = tat[i] - bt[i];

                    avgWT += wt[i];
                    avgTAT += tat[i];
                }
            }
        }

        /* If no process has arrived yet */
        if(executed == 0)
        {
            time++;
        }
    }

    printf("\nProcess\tAT\tBT\tCT\tWT\tTAT\n");

    for(i = 0; i < n; i++)
    {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               i + 1, at[i], bt[i], ct[i], wt[i], tat[i]);
    }

    printf("\nAverage Waiting Time = %.2f", avgWT / n);
    printf("\nAverage Turnaround Time = %.2f\n", avgTAT / n);
}
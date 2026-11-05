#include <stdio.h>

void priorityScheduling();
void srtf();

int main()
{
    int choice;

    do
    {
        printf("\n========== CPU Scheduling ==========\n");
        printf("1. Priority Scheduling\n");
        printf("2. Shortest Remaining Time First (SRTF)\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                priorityScheduling();
                break;

            case 2:
                srtf();
                break;

            case 3:
                printf("Program Ended.\n");
                break;

            default:
                printf("Invalid Choice!\n");
        }

    } while(choice != 3);

    return 0;
}


/*---------------- PRIORITY SCHEDULING ----------------*/

void priorityScheduling()
{
    int n, i;
    int at[20], bt[20], pr[20];
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

        printf("Priority     : ");
        scanf("%d", &pr[i]);
    }

    while(count < n)
    {
        int index = -1;
        int highest = 999;

        for(i = 0; i < n; i++)
        {
            if(at[i] <= time && completed[i] == 0 && pr[i] < highest)
            {
                highest = pr[i];
                index = i;
            }
        }

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

    printf("\nProcess\tAT\tBT\tPR\tCT\tWT\tTAT\n");

    for(i = 0; i < n; i++)
    {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               i + 1, at[i], bt[i], pr[i], ct[i], wt[i], tat[i]);
    }

    printf("\nAverage Waiting Time = %.2f", avgWT / n);
    printf("\nAverage Turnaround Time = %.2f\n", avgTAT / n);
}


/*---------------- SRTF ----------------*/

void srtf()
{
    int n, i;
    int at[20], bt[20], rt[20];
    int ct[20], wt[20], tat[20];

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

    while(completed < n)
    {
        int index = -1;
        int smallest = 999;

        for(i = 0; i < n; i++)
        {
            if(at[i] <= time && rt[i] > 0 && rt[i] < smallest)
            {
                smallest = rt[i];
                index = i;
            }
        }

        if(index == -1)
        {
            time++;
        }
        else
        {
            rt[index]--;
            time++;

            if(rt[index] == 0)
            {
                completed++;

                ct[index] = time;
                tat[index] = ct[index] - at[index];
                wt[index] = tat[index] - bt[index];

                avgWT += wt[index];
                avgTAT += tat[index];
            }
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
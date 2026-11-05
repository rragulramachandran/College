#include <stdio.h>

void display(int n, int process[], int AT[], int BT[],
             int WT[], int TAT[])
{
    float total_wt = 0, total_tat = 0;

    printf("\nP\tAT\tBT\tWT\tTAT\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\n",
        process[i], AT[i], BT[i], WT[i], TAT[i]);

        total_wt += WT[i];
        total_tat += TAT[i];
    }

    printf("\nAverage WT = %.2f", total_wt/n);
    printf("\nAverage TAT = %.2f\n", total_tat/n);
}

// 1. FCFS
void fcfs(int n, int process[], int AT[], int BT[],
          int WT[], int TAT[])
{
    int current_time = 0;

    for (int i = 0; i < n; i++) {

        if (current_time < AT[i])
            current_time = AT[i];

        WT[i] = current_time - AT[i];
        TAT[i] = WT[i] + BT[i];

        current_time += BT[i];
    }

    display(n, process, AT, BT, WT, TAT);
}

// 2. SJF Non-preemptive
void sjf(int n, int process[], int AT[], int BT[],
         int WT[], int TAT[])
{
    int current_time = 0, done[20] = {0};
    int count = 0, k;

    while (count < n) {
        k = -1;

        for (int i = 0; i < n; i++) {
            if (!done[i] && AT[i] <= current_time) {
                if (k == -1 || BT[i] < BT[k])
                    k = i;
            }
        }

        if (k == -1) {
            current_time++;
            continue;
        }

        WT[k] = current_time - AT[k];
        TAT[k] = WT[k] + BT[k];

        current_time += BT[k];

        done[k] = 1;
        count++;
    }

    display(n, process, AT, BT, WT, TAT);
}

// 3. SRTF
void srtf(int n, int process[], int AT[], int BT[],
          int RT[], int WT[], int TAT[])
{
    int current_time = 0, count = 0, k;

    for (int i = 0; i < n; i++)
        RT[i] = BT[i];

    while (count < n) {
        k = -1;

        for (int i = 0; i < n; i++) {
            if (AT[i] <= current_time && RT[i] > 0) {
                if (k == -1 || RT[i] < RT[k])
                    k = i;
            }
        }

        if (k == -1) {
            current_time++;
            continue;
        }

        RT[k]--;
        current_time++;

        if (RT[k] == 0) {
            TAT[k] = current_time - AT[k];
            WT[k] = TAT[k] - BT[k];
            count++;
        }
    }

    display(n, process, AT, BT, WT, TAT);
}

// 4. Priority Non-preemptive
void priority_np(int n, int process[], int AT[], int BT[],
                 int PR[], int WT[], int TAT[])
{
    int current_time = 0, done[20] = {0};
    int count = 0, k;

    while (count < n) {
        k = -1;

        for (int i = 0; i < n; i++) {
            if (!done[i] && AT[i] <= current_time) {
                if (k == -1 || PR[i] < PR[k])
                    k = i;
            }
        }

        if (k == -1) {
            current_time++;
            continue;
        }

        WT[k] = current_time - AT[k];
        TAT[k] = WT[k] + BT[k];

        current_time += BT[k];

        done[k] = 1;
        count++;
    }

    display(n, process, AT, BT, WT, TAT);
}

// 5. Priority Preemptive
void priority_p(int n, int process[], int AT[], int BT[],
                int PR[], int RT[], int WT[], int TAT[])
{
    int current_time = 0, count = 0, k;

    for (int i = 0; i < n; i++)
        RT[i] = BT[i];

    while (count < n) {
        k = -1;

        for (int i = 0; i < n; i++) {
            if (AT[i] <= current_time && RT[i] > 0) {
                if (k == -1 || PR[i] < PR[k])
                    k = i;
            }
        }

        if (k == -1) {
            current_time++;
            continue;
        }

        RT[k]--;
        current_time++;

        if (RT[k] == 0) {
            TAT[k] = current_time - AT[k];
            WT[k] = TAT[k] - BT[k];
            count++;
        }
    }

    display(n, process, AT, BT, WT, TAT);
}

// 6. Round Robin
void rr(int n, int process[], int AT[], int BT[],
        int RT[], int WT[], int TAT[])
{
    int q[1000], front = 0, rear = 0;
    int added[20] = {0};
    int current_time = 0, count = 0;
    int quantum, k, run;

    printf("Enter Quantum: ");
    scanf("%d", &quantum);

    for (int i = 0; i < n; i++)
        RT[i] = BT[i];

    while (count < n) {

        for (int i = 0; i < n; i++) {
            if (!added[i] && AT[i] <= current_time) {
                q[rear++] = i;
                added[i] = 1;
            }
        }

        if (front == rear) {
            current_time++;
            continue;
        }

        k = q[front++];

        if (RT[k] < quantum)
            run = RT[k];
        else
            run = quantum;

        current_time += run;
        RT[k] -= run;

        for (int i = 0; i < n; i++) {
            if (!added[i] && AT[i] <= current_time) {
                q[rear++] = i;
                added[i] = 1;
            }
        }

        if (RT[k] > 0)
            q[rear++] = k;
        else {
            TAT[k] = current_time - AT[k];
            WT[k] = TAT[k] - BT[k];
            count++;
        }
    }

    display(n, process, AT, BT, WT, TAT);
}

// MAIN
int main()
{
    int n, ch;

    int process[20], AT[20], BT[20];
    int WT[20], TAT[20], RT[20], PR[20];

    printf("Enter number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        process[i] = i + 1;

        printf("\nEnter AT and BT of P%d: ", i+1);
        scanf("%d%d", &AT[i], &BT[i]);
    }

    printf("\n1. FCFS");
    printf("\n2. SJF");
    printf("\n3. SRTF");
    printf("\n4. Priority Non-preemptive");
    printf("\n5. Priority Preemptive");
    printf("\n6. Round Robin");

    printf("\nEnter choice: ");
    scanf("%d", &ch);

    if (ch == 4 || ch == 5) {
        for (int i = 0; i < n; i++) {
            printf("Enter Priority of P%d: ", i+1);
            scanf("%d", &PR[i]);
        }
    }

    switch (ch) {
        case 1:
            fcfs(n, process, AT, BT, WT, TAT);
            break;

        case 2:
            sjf(n, process, AT, BT, WT, TAT);
            break;

        case 3:
            srtf(n, process, AT, BT, RT, WT, TAT);
            break;

        case 4:
            priority_np(n, process, AT, BT, PR, WT, TAT);
            break;

        case 5:
            priority_p(n, process, AT, BT, PR, RT, WT, TAT);
            break;

        case 6:
            rr(n, process, AT, BT, RT, WT, TAT);
            break;

        default:
            printf("Invalid choice!");
    }

    return 0;
}
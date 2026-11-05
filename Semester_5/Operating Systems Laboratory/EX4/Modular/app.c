#include <stdio.h>
#include <stdlib.h>
#include "header.h"

int main() {
    Process base[MAX], temp[MAX];
    int n, choice, tq;

    printf("====================================================\n");
    printf("         CPU Scheduling Algorithms Simulation       \n");
    printf("====================================================\n");

    printf("Enter number of processes (up to %d): ", MAX);
    scanf("%d", &n);

    if (n <= 0 || n > MAX) {
        printf("Invalid number of processes!\n");
        return 1;
    }

    printf("\nEnter Process Details (AT, BT, Priority):\n");
    printf("(Note: Lower numerical value indicates higher priority)\n");
    for (int i = 0; i < n; i++) {
        base[i].pid = i + 1;
        printf("\nProcess P%d:\n", base[i].pid);
        printf("  Arrival Time (AT) : ");
        scanf("%d", &base[i].at);
        printf("  Burst Time (BT)   : ");
        scanf("%d", &base[i].bt);
        printf("  Priority (PR)     : ");
        scanf("%d", &base[i].pr);
    }

    do {
        printf("\n------------------- SCHEDULING MENU -------------------\n");
        printf("1. First Come First Served (FCFS)\n");
        printf("2. Shortest Job First (SJF - Non-Preemptive)\n");
        printf("3. Shortest Remaining Time First (SRTF)\n");
        printf("4. Round Robin (RR)\n");
        printf("5. Priority Scheduling (Preemptive)\n");
        printf("6. Re-enter Process Dataset\n");
        printf("7. Exit\n");
        printf("-------------------------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                resetProcesses(base, temp, n);
                fcfs(temp, n);
                break;

            case 2:
                resetProcesses(base, temp, n);
                sjf(temp, n);
                break;

            case 3:
                resetProcesses(base, temp, n);
                srtf(temp, n);
                break;

            case 4:
                printf("Enter Time Quantum for Round Robin: ");
                scanf("%d", &tq);
                resetProcesses(base, temp, n);
                roundRobin(temp, n, tq);
                break;

            case 5:
                resetProcesses(base, temp, n);
                priorityPreemptive(temp, n);
                break;

            case 6:
                printf("\nEnter new number of processes (up to %d): ", MAX);
                scanf("%d", &n);
                for (int i = 0; i < n; i++) {
                    base[i].pid = i + 1;
                    printf("\nProcess P%d:\n", base[i].pid);
                    printf("  Arrival Time (AT) : ");
                    scanf("%d", &base[i].at);
                    printf("  Burst Time (BT)   : ");
                    scanf("%d", &base[i].bt);
                    printf("  Priority (PR)     : ");
                    scanf("%d", &base[i].pr);
                }
                break;

            case 7:
                printf("Exiting simulator. Good luck with your lab exam!\n");
                break;

            default:
                printf("Invalid choice! Please select 1 to 7.\n");
        }
    } while (choice != 7);

    return 0;
}

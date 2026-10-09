#include <stdio.h>
#include <stdlib.h>
#include "scheduling.h"

/* =========================================================
 * MAIN - Menu driven driver program
 * ========================================================= */
int main(void)
{
  int processlst[MAX];
  float bursttime[MAX], arrivaltime[MAX];
  float awtime, atattime, tq;
  int choice;

  printf("Enter number of processes: ");
  scanf("%d", &n);

  for (int i = 0; i < n; i++)
  {
    processlst[i] = i + 1;
    printf("Enter Arrival Time and Burst Time for P%d: ", processlst[i]);
    scanf("%f %f", &arrivaltime[i], &bursttime[i]);
  }

  do
  {
    printf("\n---------- CPU Scheduling Menu ----------\n");
    printf("1. FCFS\n");
    printf("2. SJF (Non-preemptive)\n");
    printf("3. SRTF (Preemptive SJF)\n");
    printf("4. Priority Scheduling\n");
    printf("5. Round Robin\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
      case 1:
        fcfs(processlst, bursttime, arrivaltime, &awtime, &atattime);
        printf("\nAverage Waiting Time    = %.2f\n", awtime);
        printf("Average Turnaround Time = %.2f\n", atattime);
        break;

      case 2:
        sjfs(processlst, bursttime, arrivaltime, &awtime, &atattime);
        printf("\nAverage Waiting Time    = %.2f\n", awtime);
        printf("Average Turnaround Time = %.2f\n", atattime);
        break;

      case 3:
        srtf(processlst, bursttime, arrivaltime, &awtime, &atattime);
        printf("\nAverage Waiting Time    = %.2f\n", awtime);
        printf("Average Turnaround Time = %.2f\n", atattime);
        break;

      case 4:
        for (int i = 0; i < n; i++)
        {
          printf("Enter priority for P%d (lower = higher priority): ", processlst[i]);
          scanf("%d", &priority[i]);
        }
        prioritySchedule(processlst, bursttime, arrivaltime, &awtime, &atattime);
        printf("\nAverage Waiting Time    = %.2f\n", awtime);
        printf("Average Turnaround Time = %.2f\n", atattime);
        break;

      case 5:
        printf("Enter Time Quantum: ");
        scanf("%f", &tq);
        roundRobin(processlst, bursttime, arrivaltime, &awtime, &atattime, tq);
        printf("\nAverage Waiting Time    = %.2f\n", awtime);
        printf("Average Turnaround Time = %.2f\n", atattime);
        break;

      case 0:
        printf("Exiting...\n");
        break;

      default:
        printf("Invalid choice! Try again.\n");
    }
  } while (choice != 0);

  return 0;
}

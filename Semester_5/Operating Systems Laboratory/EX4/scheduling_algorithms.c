#include <stdio.h>
#include <stdlib.h>
#include "scheduling.h"

/* ---- Globals ---- */
int n;
int priority[MAX];

/* ---- Helper to print a result table ---- */
static void printTable(int *processlst, float *bursttime, float *arrivaltime,
                       float *completion, float *waiting, float *turnaround)
{
  printf("\n%-10s%-10s%-10s%-12s%-10s%-12s\n", "Process", "Arrival", "Burst",
         "Completion", "Waiting", "Turnaround");
  for (int i = 0; i < n; i++)
  {
    printf("P%-9d%-10.2f%-10.2f%-12.2f%-10.2f%-12.2f\n", processlst[i], arrivaltime[i],
           bursttime[i], completion[i], waiting[i], turnaround[i]);
  }
}

/* =========================================================
 * 1. FCFS - First Come First Serve
 * ========================================================= */
void fcfs(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime)
{
  int idx[MAX];
  float completion[MAX], waiting[MAX], turnaround[MAX];
  for (int i = 0; i < n; i++)
  {
    idx[i] = i;
  }

  /* sort indices by arrival time (stable insertion sort) */
  for (int i = 1; i < n; i++)
  {
    int key = idx[i], j = i - 1;
    while (j >= 0 && arrivaltime[idx[j]] > arrivaltime[key])
    {
      idx[j + 1] = idx[j];
      j--;
    }
    idx[j + 1] = key;
  }

  float time = 0;
  float totalWT = 0, totalTAT = 0;
  for (int k = 0; k < n; k++)
  {
    int i = idx[k];
    if (time < arrivaltime[i])
    {
      time = arrivaltime[i];
    }
    time += bursttime[i];
    completion[i] = time;
    turnaround[i] = completion[i] - arrivaltime[i];
    waiting[i] = turnaround[i] - bursttime[i];
    totalWT += waiting[i];
    totalTAT += turnaround[i];
  }

  printf("\n===== FCFS Scheduling =====\n");
  printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);

  *awtime = totalWT / n;
  *atattime = totalTAT / n;
}

/* =========================================================
 * 2. SJF - Shortest Job First (Non-preemptive)
 * ========================================================= */
void sjfs(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime)
{
  float completion[MAX], waiting[MAX], turnaround[MAX], done[MAX];
  float remainingBurst[MAX];
  for (int i = 0; i < n; i++)
  {
    done[i] = 0;
    remainingBurst[i] = bursttime[i];
  }

  float time = 0;
  int completed = 0;
  float totalWT = 0, totalTAT = 0;

  while (completed < n)
  {
    int chosen = -1;
    float minBurst = 1e9;
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
      /* no process has arrived yet, jump time forward */
      time += 0.01;
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

/* =========================================================
 * 3. SRTF - Shortest Remaining Time First (Preemptive SJF)
 * ========================================================= */
void srtf(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime)
{
  float remaining[MAX], completion[MAX], waiting[MAX], turnaround[MAX];
  for (int i = 0; i < n; i++)
  {
    remaining[i] = bursttime[i];
  }

  int completed = 0;
  float time = 0;
  float totalWT = 0, totalTAT = 0;

  /* find max arrival + total burst to bound simulation */
  float maxArrival = 0, totalBurst = 0;
  for (int i = 0; i < n; i++)
  {
    if (arrivaltime[i] > maxArrival)
    {
      maxArrival = arrivaltime[i];
    }
    totalBurst += bursttime[i];
  }
  float limit = maxArrival + totalBurst + 1;

  while (completed < n && time < limit)
  {
    int chosen = -1;
    float minRem = 1e9;
    for (int i = 0; i < n; i++)
    {
      if (arrivaltime[i] <= time && remaining[i] > 0 && remaining[i] < minRem)
      {
        minRem = remaining[i];
        chosen = i;
      }
    }
    
    if (chosen == -1)
    {
      time += 0.01;
      continue;
    }
    
    remaining[chosen] -= 0.01;
    time += 0.01;
    if (remaining[chosen] <= 0.001)
    {
      if (remaining[chosen] < 0)
      {
        time += remaining[chosen];
        remaining[chosen] = 0;
      }
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

/* =========================================================
 * 4. Priority Scheduling (Non-preemptive, lower number = higher priority)
 * ========================================================= */
void prioritySchedule(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime)
{
  float completion[MAX], waiting[MAX], turnaround[MAX], done[MAX];
  for (int i = 0; i < n; i++)
  {
    done[i] = 0;
  }

  float time = 0;
  int completed = 0;
  float totalWT = 0, totalTAT = 0;

  while (completed < n)
  {
    int chosen = -1;
    int bestPriority = 1e9;
    for (int i = 0; i < n; i++)
    {
      if (!done[i] && arrivaltime[i] <= time && priority[i] < bestPriority)
      {
        bestPriority = priority[i];
        chosen = i;
      }
    }
    
    if (chosen == -1)
    {
      time += 0.01;
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

  printf("\n===== Priority Scheduling =====\n");
  printf("\n%-10s%-10s%-10s%-10s%-12s%-10s%-12s\n", "Process", "Arrival",
         "Burst", "Priority", "Completion", "Waiting", "Turnaround");
  for (int i = 0; i < n; i++)
  {
    printf("P%-9d%-10.2f%-10.2f%-10d%-12.2f%-10.2f%-12.2f\n", processlst[i],
           arrivaltime[i], bursttime[i], priority[i], completion[i], waiting[i],
           turnaround[i]);
  }

  *awtime = totalWT / n;
  *atattime = totalTAT / n;
}

/* =========================================================
 * 5. Round Robin
 * ========================================================= */
void roundRobin(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime, float timequantum)
{
  float remaining[MAX], completion[MAX], waiting[MAX], turnaround[MAX];
  
  for (int i = 0; i < n; i++)
  {
    remaining[i] = bursttime[i];
  }

  float time = 0;
  int completed = 0;
  float totalWT = 0, totalTAT = 0;

  while (completed < n)
  {
    int idle = 1; /* flag to check if we processed anything in this sweep */
    for (int i = 0; i < n; i++)
    {
      /* Process is ready if it hasn't finished and has arrived */
      if (remaining[i] > 0 && arrivaltime[i] <= time)
      {
        idle = 0; 
        
        float slice = (remaining[i] < timequantum) ? remaining[i] : timequantum;
        time += slice;
        remaining[i] -= slice;

        if (remaining[i] <= 0.001)
        {
          completion[i] = time;
          turnaround[i] = completion[i] - arrivaltime[i];
          waiting[i] = turnaround[i] - bursttime[i];
          totalWT += waiting[i];
          totalTAT += turnaround[i];
          completed++;
        }
      }
    }
    
    /* If no process was ready, advance time */
    if (idle)
    {
      time += 0.01;
    }
  }

  printf("\n===== Round Robin Scheduling (Quantum = %.2f) =====\n", timequantum);
  printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);

  *awtime = totalWT / n;
  *atattime = totalTAT / n;
}

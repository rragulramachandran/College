#include <stdio.h>
#include "header.h"

// Reset process state before running an algorithm
void resetProcesses(Process src[], Process dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
        dest[i].ct = 0;
        dest[i].tat = 0;
        dest[i].wt = 0;
        dest[i].rt = src[i].bt;
        dest[i].completed = 0;
    }
}

// Display table summary and averages
void displayResults(Process p[], int n, const char *algo_name) {
    float total_tat = 0, total_wt = 0;

    printf("\n========================================================================\n");
    printf("           Algorithm: %s\n", algo_name);
    printf("========================================================================\n");
    printf("| Process |  AT  |  BT  | Priority |  CT  |  TAT  |  WT  |\n");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        total_tat += p[i].tat;
        total_wt += p[i].wt;
        printf("|   P%-4d |  %2d  |  %2d  |    %2d    |  %2d  |  %3d  |  %2d  |\n",
               p[i].pid, p[i].at, p[i].bt, p[i].pr, p[i].ct, p[i].tat, p[i].wt);
    }
    printf("========================================================================\n");
    printf("Average Turnaround Time = %.2f\n", total_tat / n);
    printf("Average Waiting Time    = %.2f\n", total_wt / n);
    printf("========================================================================\n\n");
}

// ---------------- 1. First Come First Served (FCFS) ----------------
void fcfs(Process p[], int n) {
    // Sort processes by Arrival Time (tie-breaker: PID)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].at > p[j + 1].at || (p[j].at == p[j + 1].at && p[j].pid > p[j + 1].pid)) {
                Process temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }

    int current_time = 0;
    for (int i = 0; i < n; i++) {
        // Handle CPU idle time
        if (current_time < p[i].at) {
            current_time = p[i].at;
        }
        current_time += p[i].bt;
        p[i].ct = current_time;
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;
    }

    // Sort back by PID for clean display
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].pid > p[j + 1].pid) {
                Process temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }

    displayResults(p, n, "First Come First Served (FCFS)");
}

// ---------------- 2. Shortest Job First (SJF - Non-Preemptive) ----------------
void sjf(Process p[], int n) {
    int current_time = 0, completed = 0;

    while (completed < n) {
        int idx = -1;
        int min_bt = 999999;

        // Find arrived process with shortest burst time
        for (int i = 0; i < n; i++) {
            if (p[i].at <= current_time && !p[i].completed) {
                if (p[i].bt < min_bt) {
                    min_bt = p[i].bt;
                    idx = i;
                } else if (p[i].bt == min_bt) {
                    // Tie-breaker: earlier arrival time
                    if (p[i].at < p[idx].at) idx = i;
                }
            }
        }

        if (idx == -1) {
            // CPU is idle: move to the next process's arrival
            current_time++;
        } else {
            current_time += p[idx].bt;
            p[idx].ct = current_time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            p[idx].completed = 1;
            completed++;
        }
    }

    displayResults(p, n, "Shortest Job First (SJF - Non-Preemptive)");
}

// ---------------- 3. Shortest Remaining Time First (SRTF) ----------------
void srtf(Process p[], int n) {
    int current_time = 0, completed = 0;

    while (completed < n) {
        int idx = -1;
        int min_rt = 999999;

        // Find arrived process with minimum remaining time
        for (int i = 0; i < n; i++) {
            if (p[i].at <= current_time && p[i].rt > 0) {
                if (p[i].rt < min_rt) {
                    min_rt = p[i].rt;
                    idx = i;
                } else if (p[i].rt == min_rt) {
                    // Tie-breaker: earlier arrival time
                    if (p[i].at < p[idx].at) idx = i;
                }
            }
        }

        if (idx == -1) {
            current_time++;
        } else {
            p[idx].rt--;
            current_time++;

            if (p[idx].rt == 0) {
                p[idx].ct = current_time;
                p[idx].tat = p[idx].ct - p[idx].at;
                p[idx].wt = p[idx].tat - p[idx].bt;
                completed++;
            }
        }
    }

    displayResults(p, n, "Shortest Remaining Time First (SRTF)");
}

// ---------------- 4. Round Robin (RR) ----------------
void roundRobin(Process p[], int n, int tq) {
    int queue[200], front = 0, rear = 0;
    int in_queue[MAX] = {0};
    int current_time = 0, completed = 0;

    // Find the first process to arrive
    int min_at = 999999;
    for (int i = 0; i < n; i++) {
        if (p[i].at < min_at) min_at = p[i].at;
    }
    current_time = min_at;

    // Push initial arrived processes into queue
    for (int i = 0; i < n; i++) {
        if (p[i].at <= current_time) {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }

    while (completed < n) {
        if (front == rear) {
            // Queue is empty: CPU is idle
            current_time++;
            for (int i = 0; i < n; i++) {
                if (p[i].at <= current_time && p[i].rt > 0 && !in_queue[i]) {
                    queue[rear++] = i;
                    in_queue[i] = 1;
                }
            }
            continue;
        }

        int curr = queue[front++];
        int exec_time = (p[curr].rt > tq) ? tq : p[curr].rt;

        p[curr].rt -= exec_time;
        current_time += exec_time;

        // Check for any newly arrived processes during execution
        for (int i = 0; i < n; i++) {
            if (p[i].at <= current_time && p[i].rt > 0 && !in_queue[i]) {
                queue[rear++] = i;
                in_queue[i] = 1;
            }
        }

        if (p[curr].rt > 0) {
            // Process still has remaining time: re-queue it
            queue[rear++] = curr;
        } else {
            // Process finished
            p[curr].ct = current_time;
            p[curr].tat = p[curr].ct - p[curr].at;
            p[curr].wt = p[curr].tat - p[curr].bt;
            completed++;
        }
    }

    displayResults(p, n, "Round Robin (RR)");
}

// ---------------- 5. Priority Scheduling (Preemptive) ----------------
void priorityPreemptive(Process p[], int n) {
    int current_time = 0, completed = 0;

    while (completed < n) {
        int idx = -1;
        int min_pr = 999999; // Lower numerical value = higher priority

        for (int i = 0; i < n; i++) {
            if (p[i].at <= current_time && p[i].rt > 0) {
                if (p[i].pr < min_pr) {
                    min_pr = p[i].pr;
                    idx = i;
                } else if (p[i].pr == min_pr) {
                    // Tie-breaker: FCFS (earlier arrival time)
                    if (p[i].at < p[idx].at) idx = i;
                }
            }
        }

        if (idx == -1) {
            current_time++;
        } else {
            p[idx].rt--;
            current_time++;

            if (p[idx].rt == 0) {
                p[idx].ct = current_time;
                p[idx].tat = p[idx].ct - p[idx].at;
                p[idx].wt = p[idx].tat - p[idx].bt;
                completed++;
            }
        }
    }

    displayResults(p, n, "Priority Scheduling (Preemptive)");
}

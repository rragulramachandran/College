#ifndef HEADER_H
#define HEADER_H

#define MAX 10

// Process structure
typedef struct {
    int pid;        // Process ID (1 for P1, 2 for P2, etc.)
    int at;         // Arrival Time
    int bt;         // Burst Time
    int pr;         // Priority (lower value = higher priority)
    int ct;         // Completion Time
    int tat;        // Turnaround Time (CT - AT)
    int wt;         // Waiting Time (TAT - BT)
    int rt;         // Remaining Time (for preemptive algorithms)
    int completed;  // Flag: 1 if finished, 0 otherwise
} Process;

// Function declarations
void displayResults(Process p[], int n, const char *algo_name);
void resetProcesses(Process src[], Process dest[], int n);

// Scheduling Algorithms
void fcfs(Process p[], int n);
void sjf(Process p[], int n);
void srtf(Process p[], int n);
void roundRobin(Process p[], int n, int tq);
void priorityPreemptive(Process p[], int n);

#endif

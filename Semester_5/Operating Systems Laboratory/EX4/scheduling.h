#ifndef SCHEDULING_H
#define SCHEDULING_H

#define MAX 100

/* ---- Globals ---- */
extern int n;             /* number of processes            */
extern int priority[MAX]; /* priority[i] for processlst[i]  */

/* ---- Prototype Declarations ---- */
void fcfs(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime);
void sjfs(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime);
void srtf(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime);
void prioritySchedule(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime);
void roundRobin(int *processlst, float *bursttime, float *arrivaltime, float *awtime, float *atattime, float timequantum);

#endif /* SCHEDULING_H */

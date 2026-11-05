#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

/* Semaphore indexes */
#define SEM_INPUT_READY  0 /* Signaled by producer when numbers are in shared memory */
#define SEM_RESULT_READY 1 /* Signaled by consumer when addition result is ready */

/* Shared memory structure */
struct SharedData
{
    int num1;
    int num2;
    int sum;
};

/* Required for semctl() on Linux (already defined in <sys/sem.h> on macOS/BSD) */
#ifndef __APPLE__
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
#endif

/* Semaphore WAIT operation (P operation / decrement) */
static void wait_sem(int semid, int semnum)
{
    struct sembuf sb;

    sb.sem_num = semnum;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    semop(semid, &sb, 1);
}

/* Semaphore SIGNAL operation (V operation / increment) */
static void signal_sem(int semid, int semnum)
{
    struct sembuf sb;

    sb.sem_num = semnum;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    semop(semid, &sb, 1);
}

#endif

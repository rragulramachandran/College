#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

/* Buffer size */
#define SIZE 5

/* Semaphore indexes */
#define MUTEX 0
#define EMPTY 1
#define FULL  2

/* Shared memory structure */
struct SharedData 
{
    char buffer[SIZE];
    int in;
    int out;
};

/* Required for semctl() */
union semun 
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/* Semaphore WAIT operation */
static void wait_sem(int semid, int semnum)
{
    struct sembuf sb;

    sb.sem_num = semnum;
    sb.sem_op = -1;
    sb.sem_flg = 0;

    semop(semid, &sb, 1);
}

/* Semaphore SIGNAL operation */
static void signal_sem(int semid, int semnum)
{
    struct sembuf sb;

    sb.sem_num = semnum;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    semop(semid, &sb, 1);
}

#endif
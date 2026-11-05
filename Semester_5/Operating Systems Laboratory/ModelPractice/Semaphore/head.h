#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>

#define bufs 5

#define MUTEX 0
#define FULL 2
#define EMPTY 1

union semun
{
        int val;
        struct semid_ds *buf;
        unsigned short *array;
};

void wait(int semid,int sem_num){
        struct sembuf bf;
        bf.sem_num=sem_num;
        bf.sem_op=-1;
        bf.sem_flg=0;
        semop(semid,&bf,1);
}

void signal(int semid,int sem_num){
        struct sembuf sb;
        sb.sem_num=sem_num;
        sb.sem_op=1;
        sb.sem_flg=0;
        semop(semid,&sb,1);
}
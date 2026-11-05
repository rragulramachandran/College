#include"head.h"

int main()
{
        int shmid,semid;
        int semk,shmk;

        shmk=ftok(".",65);
        shmid=shmget(shmk,bufs*sizeof(int),IPC_CREAT | 0666);

        int *buffer;
        buffer=(int *)shmat(shmid,NULL,0);

        semk=ftok(".",66);
        semid=semget(semk,3,IPC_CREAT | 0666);

        union semun arg;

        arg.val=1;
        semctl(semid,MUTEX,SETVAL,arg);
        arg.val=bufs;
        semctl(semid,EMPTY,SETVAL,arg);
        arg.val=0;
        semctl(semid,FULL,SETVAL,arg);

        int a, i=0, in=0;

        while(1){
                int data;
                printf("Enter number: ");
                scanf("%d",&data);

                wait(semid,EMPTY);
                wait(semid,MUTEX);

                buffer[in]=data;
                in=(in+1)%bufs;

                signal(semid,MUTEX);
                signal(semid,FULL);
        }

        printf("FULL: %d\n",semctl(semid,FULL,GETVAL));
        printf("EMPTY: %d\n",semctl(semid,EMPTY,GETVAL));
        shmdt(buffer);
        return 0;
}
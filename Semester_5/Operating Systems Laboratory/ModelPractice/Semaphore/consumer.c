#include"head.h"

int main()
{
        int shmid,semid,semk,shmk;

        shmk=ftok(".",65);
        shmid=shmget(shmk,bufs*sizeof(int),0666);

        int *buffer;
        buffer=(int *)shmat(shmid,NULL,0);

        semk=ftok(".",66);
        semid=semget(semk,3,0666);

        int out=0;

        printf("FULL: %d\n",semctl(semid,FULL,GETVAL));
        printf("EMPTY: %d\n", semctl(semid,EMPTY,GETVAL));
        printf("MUTEX: %d\n", semctl(semid,MUTEX,GETVAL));

        while(1)
        {

                wait(semid, FULL);
                wait(semid, MUTEX);

                printf("Received: %d\n", buffer[out]);

                out = (out + 1) % bufs;

                signal(semid, MUTEX);
                signal(semid, EMPTY);

                sleep(1);
        }

        shmdt(buffer);
        shmctl(shmid,IPC_RMID,NULL);
        semctl(semid,0,IPC_RMID);

        return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct Shared
{
    int data;
    volatile int ready;
};

int main()
{
    int shmid, shmk;
    struct Shared *shm;

    shmk = ftok(".", 65);
    shmid = shmget(shmk, sizeof(struct Shared), 0666);

    shm = (struct Shared *)shmat(shmid, NULL, 0);

    while (1)
    {
        while (shm->ready == 0)
            sleep(1);

        if (shm->data == -1)
            break;

        printf("Received: %d\n", shm->data);

        shm->ready = 0;
    }

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

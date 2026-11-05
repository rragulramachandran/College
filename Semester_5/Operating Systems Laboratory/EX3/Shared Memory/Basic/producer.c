
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
    shmid = shmget(shmk, sizeof(struct Shared), IPC_CREAT | 0666);

    shm = (struct Shared *)shmat(shmid, NULL, 0);

    shm->ready = 0;

    while (1)
    {
        int data;

        printf("Enter number (-1 to stop): ");
        scanf("%d", &data);

        while (shm->ready == 1)
            sleep(1);

        shm->data = data;
        shm->ready = 1;

        if (data == -1)
            break;
    }

    shmdt(shm);

    return 0;
}

#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct Addition {
    int a;
    int b;
    int sum;
    int ready;
};

int main()
{
    struct Addition *data;
    int shmid;

    key_t key = ftok("producer.c", 65);

    if (key == -1)
    {
        perror("ftok");
        return 1;
    }

    printf("Waiting for producer...\n");

    while(1)
    {
        shmid = shmget(key, sizeof(struct Addition), 0666);

        if(shmid != -1)
        {
            break;
        }

        sleep(1);
    }

    data = (struct Addition *)shmat(shmid, NULL, 0);

    if (data == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

    while(data->ready != 1)
    {
        sleep(1);
    }

    data->sum = data->a + data->b;

    printf("Sum : %d\n", data->sum);

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

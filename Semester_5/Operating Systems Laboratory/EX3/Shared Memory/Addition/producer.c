#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>

struct Addition {
    int a;
    int b;
    int sum;
    int ready;
};

int main()
{
    struct Addition *data;

    key_t key = ftok("producer.c", 65);

    int shmid = shmget(key, sizeof(struct Addition), IPC_CREAT | 0666);

    data = (struct Addition *)shmat(shmid, NULL, 0);

    data->ready = 0;

    printf("Enter Number 1 : ");
    scanf("%d", &data->a);

    printf("Enter Number 2 : ");
    scanf("%d", &data->b);

    data->ready = 1;

    shmdt(data);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 1024

int main()
{
    int shmid;
    int i;
    char *shared;
    pid_t producer;
    pid_t consumer;

    shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }

    shared = (char *)shmat(shmid, NULL, 0);

    if (shared == (char *)-1)
    {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    producer = fork();

    if (producer < 0)
    {
        perror("fork");
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    if (producer == 0)
    {
        printf("Enter a string: ");
        fgets(shared, SIZE, stdin);
        shared[strcspn(shared, "\n")] = '\0';
        shmdt(shared);
        return 0;
    }

    wait(NULL);

    consumer = fork();

    if (consumer < 0)
    {
        perror("fork");
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    if (consumer == 0)
    {
        printf("Reversed string: ");

        for (i = (int)strlen(shared) - 1; i >= 0; i--)
        {
            printf("%c", shared[i]);
        }

        printf("\n");
        shmdt(shared);
        return 0;
    }

    wait(NULL);
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

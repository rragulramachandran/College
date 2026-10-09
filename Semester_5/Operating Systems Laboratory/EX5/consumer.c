#include "header.h"

int main()
{
    int shmid;
    int semid;
    int i;

    key_t shm_key;
    key_t sem_key;

    struct SharedData *data;

    /* Generate same keys */
    shm_key = ftok(".", 'S');
    sem_key = ftok(".", 'M');

    if (shm_key == -1 || sem_key == -1) {
        perror("ftok");
        return 1;
    }

    /* Access shared memory */
    shmid = shmget(shm_key, sizeof(struct SharedData), 0666);

    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    /* Attach shared memory */
    data = (struct SharedData *)shmat(shmid, NULL, 0);

    if (data == (struct SharedData *)-1) {
        perror("shmat");
        return 1;
    }

    /* Access semaphore set */
    semid = semget(sem_key, 3, 0666);

    if (semid == -1) {
        perror("semget");
        return 1;
    }

    printf("Consumer started...\n");

    for (i = 0; i < 10; i++) {

        char ch;

        /* Wait until item is available */
        wait_sem(semid, FULL);

        /* Enter critical section */
        wait_sem(semid, MUTEX);

        ch = data->buffer[data->out];

        data->out = (data->out + 1) % SIZE;

        /* Leave critical section */
        signal_sem(semid, MUTEX);

        /* Empty slot available */
        signal_sem(semid, EMPTY);

        printf("Consumer read: %c\tASCII value: %d\n",
               ch, (int)ch);
    }

    /* Detach */
    shmdt(data);

    /* Delete shared memory */
    shmctl(shmid, IPC_RMID, NULL);

    /* Delete semaphore set */
    semctl(semid, 0, IPC_RMID);

    printf("Consumer finished.\n");

    return 0;
}
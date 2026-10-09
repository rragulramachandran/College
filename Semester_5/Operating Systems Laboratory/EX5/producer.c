#include "header.h"

int main()
{
    int shmid;
    int semid;
    int i;

    key_t shm_key;
    key_t sem_key;

    struct SharedData *data;
    union semun u;

    /* Generate keys */
    shm_key = ftok(".", 'S');
    sem_key = ftok(".", 'M');

    if (shm_key == -1 || sem_key == -1) {
        perror("ftok");
        return 1;
    }

    /* Create shared memory */
    shmid = shmget(shm_key, sizeof(struct SharedData),
                   IPC_CREAT | 0666);

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

    data->in = 0;
    data->out = 0;

    /* Create semaphore set */
    semid = semget(sem_key, 3, IPC_CREAT | 0666);

    if (semid == -1) {
        perror("semget");
        return 1;
    }

    /* Initialize semaphores */
    u.val = 1;
    semctl(semid, MUTEX, SETVAL, u);

    u.val = SIZE;
    semctl(semid, EMPTY, SETVAL, u);

    u.val = 0;
    semctl(semid, FULL, SETVAL, u);

    printf("Producer started...\n");

    for (i = 0; i < 10; i++) {

        char ch = 'A' + i;

        /* Wait for empty slot */
        wait_sem(semid, EMPTY);

        /* Enter critical section */
        wait_sem(semid, MUTEX);

        data->buffer[data->in] = ch;

        printf("Producer produced: %c\n", ch);

        data->in = (data->in + 1) % SIZE;

        /* Leave critical section */
        signal_sem(semid, MUTEX);

        /* Item available */
        signal_sem(semid, FULL);
    }

    shmdt(data);

    printf("Producer finished.\n");

    return 0;
}
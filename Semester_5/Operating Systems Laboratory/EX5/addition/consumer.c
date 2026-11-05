#include "header.h"

int main()
{
    int shmid;
    int semid;

    key_t shm_key;
    key_t sem_key;
    struct SharedData *data;

    /* Generate same keys */
    shm_key = ftok(".", 'S');
    sem_key = ftok(".", 'M');
    if (shm_key == -1 || sem_key == -1)
    {
        perror("ftok");
        return 1;
    }

    /* Access existing shared memory */
    shmid = shmget(shm_key, sizeof(struct SharedData), 0666);
    if (shmid == -1)
    {
        perror("shmget (ensure producer is running first)");
        return 1;
    }

    /* Attach shared memory */
    data = (struct SharedData *)shmat(shmid, NULL, 0);
    if (data == (struct SharedData *)-1)
    {
        perror("shmat");
        return 1;
    }

    /* Access existing semaphore set */
    semid = semget(sem_key, 2, 0666);
    if (semid == -1)
    {
        perror("semget (ensure producer is running first)");
        return 1;
    }

    printf("=== Consumer (Adder Process) ===\n");
    printf("[Consumer] Waiting for producer to write two numbers...\n");

    /* Wait until producer signals that numbers are ready */
    wait_sem(semid, SEM_INPUT_READY);

    /* Read the two numbers */
    printf("[Consumer] Read numbers from shared memory: num1 = %d, num2 = %d\n",
           data->num1, data->num2);

    /* Add the two numbers and store the result in shared memory */
    data->sum = data->num1 + data->num2;
    printf("[Consumer] Calculated sum = %d\n", data->sum);

    /* Signal producer that result is ready */
    printf("[Consumer] Signaling producer that result is ready...\n");
    signal_sem(semid, SEM_RESULT_READY);

    /* Detach shared memory */
    shmdt(data);

    printf("[Consumer] Detached from shared memory and exiting.\n");
    return 0;
}

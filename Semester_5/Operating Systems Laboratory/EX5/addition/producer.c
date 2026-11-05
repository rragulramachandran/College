#include "header.h"

int main()
{
    int shmid;
    int semid;

    key_t shm_key;
    key_t sem_key;
    struct SharedData *data;
    union semun u;

    /* Generate keys */
    shm_key = ftok(".", 'S');
    sem_key = ftok(".", 'M');
    if (shm_key == -1 || sem_key == -1)
    {
        perror("ftok");
        return 1;
    }

    /* Create shared memory */
    shmid = shmget(shm_key, sizeof(struct SharedData), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }

    /* Attach shared memory */
    data = (struct SharedData *)shmat(shmid, NULL, 0);
    if (data == (struct SharedData *)-1)
    {
        perror("shmat");
        return 1;
    }

    /* Create semaphore set (2 semaphores: SEM_INPUT_READY and SEM_RESULT_READY) */
    semid = semget(sem_key, 2, IPC_CREAT | 0666);
    if (semid == -1)
    {
        perror("semget");
        return 1;
    }

    /* Initialize both semaphores to 0 */
    u.val = 0;
    semctl(semid, SEM_INPUT_READY, SETVAL, u);
    semctl(semid, SEM_RESULT_READY, SETVAL, u);

    printf("=== Producer (Input Process) ===\n");
    printf("Enter first number: ");
    if (scanf("%d", &data->num1) != 1)
    {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }
    printf("Enter second number: ");
    if (scanf("%d", &data->num2) != 1)
    {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }

    printf("[Producer] Wrote numbers %d and %d to shared memory.\n", data->num1, data->num2);
    printf("[Producer] Signaling consumer to calculate sum...\n");

    /* Signal consumer that input numbers are ready in shared memory */
    signal_sem(semid, SEM_INPUT_READY);

    /* Wait for consumer to finish addition and signal back */
    printf("[Producer] Waiting for consumer to compute result...\n");
    wait_sem(semid, SEM_RESULT_READY);

    /* Read computed result */
    printf("[Producer] Received result from consumer: %d + %d = %d\n",
           data->num1, data->num2, data->sum);

    /* Detach shared memory */
    shmdt(data);

    /* Cleanup IPC resources */
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    printf("[Producer] Cleaned up IPC resources and exiting.\n");
    return 0;
}

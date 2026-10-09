#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX 100

struct SharedData
{
    int n;
    int numbers[MAX];
    int sums[4];
};

void find_sum(struct SharedData *data, int process_number)
{
    int i;
    int start;
    int end;
    int sum;
    int part;

    part = data->n / 4;
    start = process_number * part;
    end = start + part;
    sum = 0;

    for (i = start; i < end; i++)
    {
        sum = sum + data->numbers[i];
    }

    data->sums[process_number] = sum;
}

int main()
{
    int shmid;
    int i;
    struct SharedData *data;
    pid_t p2;
    pid_t p3;
    pid_t p4;

    shmid = shmget(IPC_PRIVATE, sizeof(struct SharedData), IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }

    data = (struct SharedData *)shmat(shmid, NULL, 0);

    if (data == (struct SharedData *)-1)
    {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    printf("Enter number of elements: ");
    scanf("%d", &data->n);

    if (data->n <= 0 || data->n > MAX || data->n % 4 != 0)
    {
        printf("Enter a positive number divisible by 4 and not greater than %d\n", MAX);
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    printf("Enter %d numbers:\n", data->n);

    for (i = 0; i < data->n; i++)
    {
        scanf("%d", &data->numbers[i]);
    }

    for (i = 0; i < 4; i++)
    {
        data->sums[i] = 0;
    }

    find_sum(data, 0);

    p2 = fork();

    if (p2 < 0)
    {
        perror("fork");
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    if (p2 == 0)
    {
        find_sum(data, 1);
        p3 = fork();

        if (p3 < 0)
        {
            perror("fork");
            shmdt(data);
            return 1;
        }

        if (p3 == 0)
        {
            find_sum(data, 2);
            p4 = fork();

            if (p4 < 0)
            {
                perror("fork");
                shmdt(data);
                return 1;
            }

            if (p4 == 0)
            {
                find_sum(data, 3);
                shmdt(data);
                return 0;
            }

            wait(NULL);
            shmdt(data);
            return 0;
        }

        wait(NULL);
        shmdt(data);
        return 0;
    }

    wait(NULL);

    for (i = 0; i < 4; i++)
    {
        printf("Sum by P%d: %d\n", i + 1, data->sums[i]);
    }

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

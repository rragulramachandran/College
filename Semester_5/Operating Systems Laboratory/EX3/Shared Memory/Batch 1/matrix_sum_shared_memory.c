#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX 10

struct MatrixData
{
    int rows;
    int cols;
    int a[MAX][MAX];
    int b[MAX][MAX];
};

int main()
{
    int shmid;
    int i;
    int j;
    struct MatrixData *data;
    pid_t child;

    shmid = shmget(IPC_PRIVATE, sizeof(struct MatrixData), IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }

    data = (struct MatrixData *)shmat(shmid, NULL, 0);

    if (data == (struct MatrixData *)-1)
    {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    printf("Enter number of rows and columns: ");
    scanf("%d %d", &data->rows, &data->cols);

    if (data->rows <= 0 || data->rows > MAX || data->cols <= 0 || data->cols > MAX)
    {
        printf("Invalid matrix size\n");
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    printf("Enter elements of first matrix:\n");

    for (i = 0; i < data->rows; i++)
    {
        for (j = 0; j < data->cols; j++)
        {
            scanf("%d", &data->a[i][j]);
        }
    }

    printf("Enter elements of second matrix:\n");

    for (i = 0; i < data->rows; i++)
    {
        for (j = 0; j < data->cols; j++)
        {
            scanf("%d", &data->b[i][j]);
        }
    }

    child = fork();

    if (child < 0)
    {
        perror("fork");
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    if (child == 0)
    {
        printf("Sum of the matrices:\n");

        for (i = 0; i < data->rows; i++)
        {
            for (j = 0; j < data->cols; j++)
            {
                printf("%d ", data->a[i][j] + data->b[i][j]);
            }

            printf("\n");
        }

        shmdt(data);
        return 0;
    }

    wait(NULL);
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

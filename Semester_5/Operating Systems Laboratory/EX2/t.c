#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1)
    {
        perror("pipe1 failed");
        exit(1);
    }
    if (pipe(pipe2) == -1)
    {
        perror("pipe2 failed");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0)
    {
        printf("[P2] PID = %d, PPID = %d\n", getpid(), getppid());
        close(pipe1[1]);
        close(pipe2[0]);
        int n;
        read(pipe1[0], &n, sizeof(int));
        close(pipe1[0]);
        printf("[P2] Received question from P1: Find sum of first %d natural numbers\n", n);
        int sum = n * (n + 1) / 2;
        printf("[P2] Calculated sum = %d\n", sum);
        printf("[P2] Sending result back to P1 through pipe2\n");
        write(pipe2[1], &sum, sizeof(int));
        close(pipe2[1]);
        exit(0);
    }
    else
    {
        printf("[P1] PID = %d, PPID = %d\n", getpid(), getppid());
        close(pipe1[0]);
        close(pipe2[1]);
        int n;
        printf("[P1] Enter a number N to find sum of first N natural numbers: ");
        scanf("%d", &n);
        printf("[P1] Sending question to P2: \"Find the sum of first %d natural numbers\"\n", n);
        write(pipe1[1], &n, sizeof(int));
        close(pipe1[1]);
        int sum;
        read(pipe2[0], &sum, sizeof(int));
        close(pipe2[0]);
        printf("[P1] Received response from P2: Sum of first %d natural numbers = %d\n", n, sum);
        wait(NULL);
    }

    return 0;
}
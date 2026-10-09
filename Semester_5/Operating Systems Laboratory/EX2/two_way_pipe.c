
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int reverseNumber(int num)
{
    int rev = 0;
    while (num != 0)
    {
        rev = rev * 10 + num % 10;
        num /= 10;
    }
    return rev;
}

int main()
{
    setvbuf(stdin, NULL, _IONBF, 0);
    int pipe1[2]; // P1 -> P2
    int pipe2[2]; // P2 -> P1

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

    if (pid > 0)
    {
        printf("[P1] PID = %d, PPID = %d\n", getpid(), getppid());

        close(pipe1[0]);
        close(pipe2[1]);

        int n;
        printf("[P1] Enter a number N to find sum of first N natural numbers: ");
        scanf("%d", &n);

        printf("[P1] Sending question to P2: \"Find the sum of first %d natural numbers\"\n", n);
        write(pipe1[1], &n, sizeof(int));

        int sum;
        read(pipe2[0], &sum, sizeof(int));
        printf("[P1] Received response from P2: Sum of first %d natural numbers = %d\n", n, sum);

        // Second exchange: P1 receives number from P2 to reverse
        int num_to_reverse;
        read(pipe2[0], &num_to_reverse, sizeof(int));
        printf("[P1] Received question from P2: \"Reverse the number %d\"\n", num_to_reverse);

        int reversed_num = reverseNumber(num_to_reverse);
        printf("[P1] Calculated reversed number = %d\n", reversed_num);

        printf("[P1] Sending answer back to P2 through pipe1\n");
        write(pipe1[1], &reversed_num, sizeof(int));

        close(pipe1[1]);
        close(pipe2[0]);

        wait(NULL);
    }
    else
    {
        printf("[P2] PID = %d, PPID = %d\n", getpid(), getppid());

        close(pipe1[1]);
        close(pipe2[0]);

        int n;
        read(pipe1[0], &n, sizeof(int));

        printf("[P2] Received question from P1: Find sum of first %d natural numbers\n", n);

        int sum = n * (n + 1) / 2;
        printf("[P2] Calculated sum = %d\n", sum);

        printf("[P2] Sending result back to P1 through pipe2\n");
        write(pipe2[1], &sum, sizeof(int));

        // Second exchange: P2 asks P1 to reverse a number
        int num_to_reverse;
        printf("[P2] Enter a number to reverse: ");
        scanf("%d", &num_to_reverse);

        printf("[P2] Sending question to P1: \"Reverse the number %d\"\n", num_to_reverse);
        write(pipe2[1], &num_to_reverse, sizeof(int));

        int reversed_num;
        read(pipe1[0], &reversed_num, sizeof(int));
        printf("[P2] Received response from P1: Reversed number = %d\n", reversed_num);

        close(pipe1[0]);
        close(pipe2[1]);
    }

    return 0;
}

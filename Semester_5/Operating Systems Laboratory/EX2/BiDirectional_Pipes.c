#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipe1[2]; // pipe1: Parent (P1) -> Child (P2)
    int pipe2[2]; // pipe2: Child (P2) -> Parent (P1)

    // Create both pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid > 0) {
        // ================= Process P1 (Parent) =================
        // Close unused pipe ends:
        close(pipe1[0]); // P1 writes to pipe1, doesn't read from it
        close(pipe2[1]); // P1 reads from pipe2, doesn't write to it

        printf("[Parent P1] PID: %d, PPID: %d (Started)\n", getpid(), getppid());

        // ----------------- PHASE 1: Sum of N Natural Numbers -----------------
        int n;
        printf("\n--- Phase 1: Sum of First N Natural Numbers ---\n");
        printf("[Parent P1] Enter an integer N: ");
        scanf("%d", &n);

        // Send N to Child (P2) via pipe1
        write(pipe1[1], &n, sizeof(n));

        // Read calculated sum from Child (P2) via pipe2
        int sum;
        read(pipe2[0], &sum, sizeof(sum));
        printf("[Parent P1] Received calculated sum from P2: %d\n", sum);

        // ----------------- PHASE 2: Reverse of a Number -----------------
        int num_to_reverse;
        // Read number sent by Child (P2) via pipe2
        read(pipe2[0], &num_to_reverse, sizeof(num_to_reverse));

        // Reverse the digits
        int reversed = 0, temp = num_to_reverse;
        while (temp != 0) {
            reversed = reversed * 10 + (temp % 10);
            temp /= 10;
        }

        printf("[Parent P1] Received %d from P2, computed reverse = %d. Sending back...\n", 
               num_to_reverse, reversed);

        // Send reversed number back to Child (P2) via pipe1
        write(pipe1[1], &reversed, sizeof(reversed));

        // Close remaining pipe ends
        close(pipe1[1]);
        close(pipe2[0]);

        // Wait for Child (P2) to terminate
        wait(NULL);

    } else {
        // ================= Process P2 (Child) =================
        // Close unused pipe ends:
        close(pipe1[1]); // P2 reads from pipe1, doesn't write to it
        close(pipe2[0]); // P2 writes to pipe2, doesn't read from it

        printf("[Child P2]  PID: %d, PPID: %d (Started)\n", getpid(), getppid());

        // ----------------- PHASE 1: Sum of N Natural Numbers -----------------
        int n;
        // Read N from Parent (P1) via pipe1
        read(pipe1[0], &n, sizeof(n));

        // Compute sum using formula: N * (N + 1) / 2
        int sum = (n * (n + 1)) / 2;
        printf("[Child P2]  Received N = %d. Computed sum = %d. Sending to P1...\n", n, sum);

        // Send sum to Parent (P1) via pipe2
        write(pipe2[1], &sum, sizeof(sum));

        // ----------------- PHASE 2: Reverse of a Number -----------------
        int num;
        printf("\n--- Phase 2: Reverse of a Number ---\n");
        printf("[Child P2]  Enter an integer to reverse: ");
        scanf("%d", &num);

        // Send number to Parent (P1) via pipe2
        write(pipe2[1], &num, sizeof(num));

        // Read reversed number from Parent (P1) via pipe1
        int reversed_num;
        read(pipe1[0], &reversed_num, sizeof(reversed_num));
        printf("[Child P2]  Received reversed number from P1: %d\n\n", reversed_num);

        // Close remaining pipe ends
        close(pipe1[0]);
        close(pipe2[1]);

        exit(0); // Child terminates
    }

    return 0;
}

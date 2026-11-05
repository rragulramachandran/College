#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int n;

    // 1. Take input from user
    printf("Enter the number of elements (N): ");
    scanf("%d", &n);

    int arr[n];
    printf("Enter %d integers: ", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    pid_t pid1, pid2;

    // 2. Create Child 1 (P2)
    pid1 = fork();

    if (pid1 < 0) {
        perror("Fork 1 failed");
        exit(1);
    }

    if (pid1 == 0) {
        // ================= Child 1 (P2): Odd Sum =================
        int odd_sum = 0;
        for (int i = 0; i < n; i++) {
            if (arr[i] % 2 != 0) {
                odd_sum += arr[i];
            }
        }
        printf("\n[Child 1 - P2] PID: %d, PPID: %d | Sum of Odd Numbers  = %d\n", 
               getpid(), getppid(), odd_sum);
        exit(0); // Terminate Child 1
    } else {
        // Parent waits for Child 1 to finish (coordinating execution)
        wait(NULL);

        // 3. Create Child 2 (P3)
        pid2 = fork();

        if (pid2 < 0) {
            perror("Fork 2 failed");
            exit(1);
        }

        if (pid2 == 0) {
            // ================= Child 2 (P3): Even Sum =================
            int even_sum = 0;
            for (int i = 0; i < n; i++) {
                if (arr[i] % 2 == 0) {
                    even_sum += arr[i];
                }
            }
            printf("[Child 2 - P3] PID: %d, PPID: %d | Sum of Even Numbers = %d\n", 
                   getpid(), getppid(), even_sum);
            exit(0); // Terminate Child 2
        } else {
            // Parent waits for Child 2 to finish
            wait(NULL);

            // ================= Parent (P1): Total Sum =================
            int total_sum = 0;
            for (int i = 0; i < n; i++) {
                total_sum += arr[i];
            }
            printf("[Parent  - P1] PID: %d, PPID: %d | Total Sum           = %d\n\n", 
                   getpid(), getppid(), total_sum);
        }
    }

    return 0;
}

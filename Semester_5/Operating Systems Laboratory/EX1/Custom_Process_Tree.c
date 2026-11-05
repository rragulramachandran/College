#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // ----------------- P1 (Root) -----------------
    printf("Process P1 (Root)        -> PID: %d, PPID: %d\n", getpid(), getppid());

    // P1 creates P2
    pid_t p2 = fork();
    if (p2 < 0) { perror("Fork failed for P2"); exit(1); }

    if (p2 == 0) {
        // ================= Inside P2 =================
        printf("Process P2 (Child of P1) -> PID: %d, PPID: %d\n", getpid(), getppid());

        // P2 creates P3
        pid_t p3 = fork();
        if (p3 < 0) { perror("Fork failed for P3"); exit(1); }

        if (p3 == 0) {
            // ================= Inside P3 =================
            printf("Process P3 (Child of P2) -> PID: %d, PPID: %d\n", getpid(), getppid());

            // P3 creates P4
            pid_t p4 = fork();
            if (p4 < 0) { perror("Fork failed for P4"); exit(1); }

            if (p4 == 0) {
                // ================= Inside P4 =================
                printf("Process P4 (Child of P3) -> PID: %d, PPID: %d\n", getpid(), getppid());

                // P4 creates P7
                pid_t p7 = fork();
                if (p7 < 0) { perror("Fork failed for P7"); exit(1); }

                if (p7 == 0) {
                    // ================= Inside P7 =================
                    printf("Process P7 (Child of P4) -> PID: %d, PPID: %d\n", getpid(), getppid());

                    // --- P7 creates Left Child: P8 ---
                    pid_t p8 = fork();
                    if (p8 < 0) { perror("Fork failed for P8"); exit(1); }

                    if (p8 == 0) {
                        // Leaf node P8
                        printf("Process P8 (Child of P7) -> PID: %d, PPID: %d\n", getpid(), getppid());
                        exit(0);
                    }

                    // P7 waits for P8
                    wait(NULL);

                    // --- P7 creates Right Child: P6 ---
                    pid_t p6 = fork();
                    if (p6 < 0) { perror("Fork failed for P6"); exit(1); }

                    if (p6 == 0) {
                        // ================= Inside P6 =================
                        printf("Process P6 (Child of P7) -> PID: %d, PPID: %d\n", getpid(), getppid());

                        // P6 creates Child: P5
                        pid_t p5 = fork();
                        if (p5 < 0) { perror("Fork failed for P5"); exit(1); }

                        if (p5 == 0) {
                            // Leaf node P5
                            printf("Process P5 (Child of P6) -> PID: %d, PPID: %d\n", getpid(), getppid());
                            exit(0);
                        }

                        // P6 waits for P5
                        wait(NULL);
                        exit(0); // P6 exits
                    }

                    // P7 waits for P6
                    wait(NULL);
                    exit(0); // P7 exits
                }

                // P4 waits for P7
                wait(NULL);
                exit(0); // P4 exits
            }

            // P3 waits for P4
            wait(NULL);
            exit(0); // P3 exits
        }

        // P2 waits for P3
        wait(NULL);
        exit(0); // P2 exits
    }

    // P1 waits for P2
    wait(NULL);

    return 0;
}

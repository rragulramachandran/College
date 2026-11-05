#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;

    // Step 1: P1 creates child P2
    pid1 = fork();

    if (pid1 < 0) {
        perror("Fork 1 failed");
        exit(1);
    }

    if (pid1 == 0) {
        // ----------------- Inside P2 (Child) -----------------
        
        // Step 2: P2 creates grandchild P3
        pid2 = fork();

        if (pid2 < 0) {
            perror("Fork 2 failed");
            exit(1);
        }

        if (pid2 == 0) {
            // ----------------- Inside P3 (Grandchild) -----------------
            printf("Process P3 (Grandchild) -> PID: %d, PPID: %d\n", getpid(), getppid());
            exit(0); // P3 terminates
        } else {
            // P2 waits for its child P3 to finish
            wait(NULL);
            printf("Process P2 (Child)      -> PID: %d, PPID: %d\n", getpid(), getppid());
            exit(0); // P2 terminates
        }

    } else {
        // ----------------- Inside P1 (Parent) -----------------
        // P1 waits for its child P2 to finish
        wait(NULL);
        printf("Process P1 (Parent)     -> PID: %d, PPID: %d\n", getpid(), getppid());
    }

    return 0;
}

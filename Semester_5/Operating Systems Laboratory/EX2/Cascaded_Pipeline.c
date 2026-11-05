#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipe1[2]; // pipe1: P1 -> P2
    char input_str[100];

    // Create pipe1
    if (pipe(pipe1) == -1) {
        perror("Pipe 1 creation failed");
        exit(1);
    }

    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("Fork 1 failed");
        exit(1);
    }

    if (pid1 > 0) {
        // ================= Process P1 (Parent) =================
        close(pipe1[0]); // Close unused read end of pipe1

        printf("[P1] Enter a string (max 80 chars): ");
        fgets(input_str, sizeof(input_str), stdin);

        // Remove trailing newline character
        input_str[strcspn(input_str, "\n")] = '\0';

        // Send string through pipe1 to P2 (including null terminator)
        write(pipe1[1], input_str, strlen(input_str) + 1);
        close(pipe1[1]); // Close write end after sending

        // Wait for child P2 to terminate
        wait(NULL);

    } else {
        // ================= Process P2 (Child) =================
        close(pipe1[1]); // Close unused write end of pipe1

        char p2_str[100];
        // Read string from P1 via pipe1
        read(pipe1[0], p2_str, sizeof(p2_str));
        close(pipe1[0]); // Close read end

        // 1. Count vowels
        int vowels = 0;
        for (int i = 0; p2_str[i] != '\0'; i++) {
            char ch = tolower(p2_str[i]);
            if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') {
                vowels++;
            }
        }
        printf("\n[P2] Received string : \"%s\"\n", p2_str);
        printf("[P2] Total Vowels    : %d\n", vowels);

        // 2. Create pipe2: P2 -> P3
        int pipe2[2];
        if (pipe(pipe2) == -1) {
            perror("Pipe 2 creation failed");
            exit(1);
        }

        pid_t pid2 = fork();

        if (pid2 < 0) {
            perror("Fork 2 failed");
            exit(1);
        }

        if (pid2 > 0) {
            // Inside P2: Forward original string to P3
            close(pipe2[0]); // Close unused read end of pipe2

            write(pipe2[1], p2_str, strlen(p2_str) + 1);
            close(pipe2[1]); // Close write end

            // P2 waits for grandchild P3 to finish
            wait(NULL);
            exit(0);

        } else {
            // ================= Process P3 (Grandchild) =================
            close(pipe2[1]); // Close unused write end of pipe2

            char p3_str[100];
            // Read string forwarded from P2 via pipe2
            read(pipe2[0], p3_str, sizeof(p3_str));
            close(pipe2[0]); // Close read end

            // Check if string is a Palindrome (case-insensitive)
            int len = strlen(p3_str);
            int is_palindrome = 1;

            for (int i = 0; i < len / 2; i++) {
                if (tolower(p3_str[i]) != tolower(p3_str[len - 1 - i])) {
                    is_palindrome = 0;
                    break;
                }
            }

            printf("\n[P3] Received string : \"%s\"\n", p3_str);
            if (is_palindrome) {
                printf("[P3] Palindrome Test : YES, it is a Palindrome (case-insensitive)\n\n");
            } else {
                printf("[P3] Palindrome Test : NO, it is NOT a Palindrome\n\n");
            }

            exit(0); // P3 exits
        }
    }

    return 0;
}

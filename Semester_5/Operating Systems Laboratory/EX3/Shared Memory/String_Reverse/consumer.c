#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_SIZE 1024

int main() {
    // Step 1: Generate the exact same IPC key using ftok()
    key_t key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok failed (Make sure Producer has run first)");
        exit(1);
    }

    // Step 2: Locate the existing shared memory segment
    int shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid < 0) {
        perror("shmget failed (Shared memory segment not found)");
        exit(1);
    }
    printf("[Consumer] Attached to Shared Memory ID: %d\n", shmid);

    // Step 3: Attach to the shared memory segment
    char *str = (char *)shmat(shmid, NULL, 0);
    if (str == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Step 4: Read the string written by the producer
    printf("[Consumer] String read from shared memory: \"%s\"\n", str);

    // Step 5: Identify and count vowels in the string
    int vowel_count = 0;
    printf("[Consumer] Vowels identified: ");
    for (int i = 0; str[i] != '\0'; i++) {
        char ch = tolower(str[i]);
        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') {
            printf("%c ", str[i]);
            vowel_count++;
        }
    }
    printf("\n[Consumer] Total vowel count: %d\n", vowel_count);

    // Step 6: Detach from shared memory
    shmdt(str);
    printf("[Consumer] Detached from shared memory. Exiting cleanly.\n");

    return 0;
}

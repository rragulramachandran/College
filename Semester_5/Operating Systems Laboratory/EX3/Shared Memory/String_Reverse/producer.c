#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_SIZE 1024

int main() {
    // Step 1: Create a temporary file for ftok() if it doesn't already exist
    FILE *fp = fopen("shmfile", "w");
    if (fp) fclose(fp);

    // Step 2: Generate a unique IPC key
    key_t key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    // Step 3: Allocate shared memory segment of size 1024 bytes
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    printf("[Producer] Shared Memory created with ID: %d (Key: %d)\n", shmid, key);

    // Step 4: Attach process to the shared memory segment
    char *str = (char *)shmat(shmid, NULL, 0);
    if (str == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Step 5: Read string from user and write to shared memory
    printf("[Producer] Enter a string: ");
    fgets(str, SHM_SIZE, stdin);
    str[strcspn(str, "\n")] = '\0'; // Remove trailing newline

    printf("[Producer] Data written to shared memory: \"%s\"\n", str);

    // Step 6: Detach from shared memory
    shmdt(str);

    // Step 7: Wait for user confirmation before destroying shared memory
    printf("\n[Producer] Waiting for Consumer to read the data...\n");
    printf("[Producer] Press [ENTER] AFTER Consumer has finished reading: ");
    getchar();

    // Step 8: Deallocate / destroy the shared memory segment
    shmctl(shmid, IPC_RMID, NULL);
    remove("shmfile"); // Clean up temporary key file
    printf("[Producer] Shared memory deallocated and cleaned up successfully.\n");

    return 0;
}

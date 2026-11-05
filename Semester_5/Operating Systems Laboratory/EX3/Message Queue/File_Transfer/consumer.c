#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define CHUNK_SIZE 1024

// Exact same message structure
struct msg_buffer {
    long msg_type;
    int last;
    int size;
    char text[CHUNK_SIZE];
};

int main() {
    // Step 1: Prompt for destination file name
    char dest_filename[256];
    printf("[Consumer] Enter destination file name to save: ");
    scanf("%255s", dest_filename);

    // Step 2: Generate same key using ftok(".", 42)
    key_t key = ftok(".", 42);
    if (key == -1) {
        perror("ftok failed (Make sure Producer has run first)");
        exit(1);
    }

    // Step 3: Access existing message queue
    int msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget failed (Message queue not found)");
        exit(1);
    }
    printf("[Consumer] Connected to Message Queue ID: %d\n", msgid);

    // Step 4: Open destination file for writing
    FILE *fp = fopen(dest_filename, "wb");
    if (!fp) {
        perror("Failed to open destination file");
        exit(1);
    }

    // Step 5: Continuously receive messages using msgrcv()
    struct msg_buffer msg;
    int chunk_count = 0;

    while (1) {
        // Wait and receive message of type 1
        if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0) == -1) {
            perror("msgrcv failed");
            fclose(fp);
            exit(1);
        }

        // Check if EOF / End-of-Transmission signal received
        if (msg.last == 1) {
            printf("[Consumer] Received End-of-Transmission signal (last = 1).\n");
            break;
        }

        // Write received chunk into destination file
        fwrite(msg.text, 1, msg.size, fp);
        chunk_count++;
        printf("[Consumer] Received and wrote chunk #%d (%d bytes)\n", chunk_count, msg.size);
    }

    fclose(fp);
    printf("[Consumer] File successfully saved as '%s'.\n", dest_filename);

    // Step 6: Destroy message queue using msgctl
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl IPC_RMID failed");
        exit(1);
    }
    printf("[Consumer] Message queue destroyed successfully.\n");

    return 0;
}

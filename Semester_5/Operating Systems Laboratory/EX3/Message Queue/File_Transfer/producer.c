#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define CHUNK_SIZE 1024

// Message structure
struct msg_buffer {
    long msg_type;
    int last;                // 0 for data, 1 for EOF
    int size;                // Actual bytes read
    char text[CHUNK_SIZE];
};

int main() {
    // Step 1: Generate unique IPC key using ftok(".", 42)
    key_t key = ftok(".", 42);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    // Step 2: Create message queue
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }
    printf("[Producer] Message Queue created with ID: %d\n", msgid);

    // Step 3: Prompt for source file name
    char filename[256];
    printf("[Producer] Enter the source file name to transfer: ");
    scanf("%255s", filename);

    // Step 4: Open source file
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open source file");
        exit(1);
    }

    // Step 5: Read file in chunks and send via msgsnd()
    struct msg_buffer msg;
    msg.msg_type = 1;

    int bytes_read;
    int chunk_count = 0;

    while ((bytes_read = fread(msg.text, 1, CHUNK_SIZE, fp)) > 0) {
        msg.last = 0;
        msg.size = bytes_read;

        // msgsnd takes size of message excluding 'msg_type'
        if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("msgsnd failed");
            fclose(fp);
            exit(1);
        }
        chunk_count++;
        printf("[Producer] Sent chunk #%d (%d bytes)\n", chunk_count, bytes_read);
    }

    fclose(fp);

    // Step 6: Send End-of-Transmission signal (last = 1)
    msg.last = 1;
    msg.size = 0;
    if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
        perror("msgsnd (EOF) failed");
        exit(1);
    }
    printf("[Producer] Sent End-of-Transmission signal (last = 1).\n");
    printf("[Producer] File transfer finished successfully.\n");

    return 0;
}

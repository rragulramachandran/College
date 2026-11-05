#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_TEXT 512

// Message structure
struct message {
    long msg_type;              // Message type (must be > 0)
    char msg_text[MAX_TEXT];    // Message content
};

int main() {
    // Step 1: Generate unique IPC key using ftok("producer.c", 42)
    key_t key = ftok("producer.c", 42);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    // Step 2: Create message queue with read/write permissions
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }
    printf("[Producer] Message Queue created with ID: %d (Key: %d)\n", msgid, key);

    // Step 3: Prompt user for sentence or message
    struct message msg;
    msg.msg_type = 1; // Assign message type 1

    printf("[Producer] Enter a sentence or message: ");
    if (fgets(msg.msg_text, MAX_TEXT, stdin) == NULL) {
        perror("fgets failed");
        exit(1);
    }

    // Remove trailing newline character
    msg.msg_text[strcspn(msg.msg_text, "\n")] = '\0';

    // Step 4: Send message into queue using msgsnd()
    // Size passed is the size of data excluding 'msg_type'
    if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }

    // Step 5: Print transmitted message and terminate
    printf("[Producer] Transmitted message: \"%s\"\n", msg.msg_text);
    printf("[Producer] Message sent successfully. Terminating.\n");

    return 0;
}

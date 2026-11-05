#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_TEXT 512

// Exact same message structure
struct message {
    long msg_type;
    char msg_text[MAX_TEXT];
};

// Function to count words delimited by whitespace (spaces, tabs, newlines)
int countWords(const char *str) {
    int count = 0;
    int in_word = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (isspace((unsigned char)str[i])) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
    }
    return count;
}

int main() {
    // Step 1: Derive identical IPC key using ftok("producer.c", 42)
    key_t key = ftok("producer.c", 42);
    if (key == -1) {
        perror("ftok failed (Ensure producer.c exists in current directory)");
        exit(1);
    }

    // Step 2: Connect to existing message queue
    int msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget failed (Run producer first)");
        exit(1);
    }
    printf("[Consumer] Connected to Message Queue ID: %d (Key: %d)\n", msgid, key);

    // Step 3: Receive message of type 1 using msgrcv()
    struct message msg;
    if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 1, 0) == -1) {
        perror("msgrcv failed");
        exit(1);
    }

    // Step 4: Display received message text
    printf("[Consumer] Received message: \"%s\"\n", msg.msg_text);

    // Step 5: Count and display total number of words
    int total_words = countWords(msg.msg_text);
    printf("[Consumer] Total number of words: %d\n", total_words);

    // Step 6: Destroy message queue using msgctl() with IPC_RMID
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl IPC_RMID failed");
        exit(1);
    }
    printf("[Consumer] Message queue deallocated and cleaned up from kernel.\n");

    return 0;
}

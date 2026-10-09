#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct message_buffer
{
    long message_type;
    char message_text[1024];
};

int main()
{
    key_t key;
    int message_queue_id;
    char dest_filename[256];
    
    printf("Enter destination filename: ");
    scanf("%s", dest_filename);
    
    key = ftok(".", 66);
    // IPC_CREAT allows the consumer to wait even if the producer hasn't been run yet.
    // It creates the queue, and msgrcv will block (wait with blinking prompt) until a message arrives.
    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    
    struct message_buffer message;
    FILE *file;
    
    file = fopen(dest_filename, "w");
    if (file == NULL)
    {
        printf("Consumer Error: Cannot create destination file.\n");
        return 1;
    }
    
    printf("Waiting for producer to send data...\n");
    
    while (1)
    {
        // This will block and wait for the producer to send a message
        msgrcv(message_queue_id, &message, sizeof(message.message_text), 1, 0);
        
        if (strcmp(message.message_text, "ERROR_FILE_NOT_FOUND") == 0 ||
            strcmp(message.message_text, "ERROR_FILE_NOT_READABLE") == 0 ||
            strcmp(message.message_text, "ERROR_FILE_OPEN") == 0)
        {
            fclose(file);
            remove(dest_filename);
            // Clean up the message queue before exiting
            msgctl(message_queue_id, IPC_RMID, NULL);
            return 1;
        }
        
        if (strcmp(message.message_text, "EOF") == 0)
        {
            break;
        }
        
        fputs(message.message_text, file);
    }
    
    fclose(file);
    printf("Consumer saved data to %s.\n", dest_filename);
    
    // Clean up the message queue
    msgctl(message_queue_id, IPC_RMID, NULL);
    
    return 0;
}

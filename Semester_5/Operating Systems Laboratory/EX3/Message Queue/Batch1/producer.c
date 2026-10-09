#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>

struct message_buffer
{
    long message_type;
    char message_text[1024];
};

int main()
{
    key_t key;
    int message_queue_id;
    char src_filename[256];
    
    printf("Enter source filename: ");
    scanf("%s", src_filename);
    
    key = ftok(".", 66);
    // IPC_CREAT ensures the queue is created if it doesn't exist
    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    
    struct message_buffer message;
    FILE *file;
    
    message.message_type = 1;
    
    if (access(src_filename, F_OK) == -1)
    {
        printf("Error: Source file does not exist.\n");
        strcpy(message.message_text, "ERROR_FILE_NOT_FOUND");
        msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
        return 1;
    }
    
    if (access(src_filename, R_OK) == -1)
    {
        printf("Error: Source file is not readable.\n");
        strcpy(message.message_text, "ERROR_FILE_NOT_READABLE");
        msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
        return 1;
    }
    
    file = fopen(src_filename, "r");
    if (file == NULL)
    {
        printf("Error: Cannot open source file.\n");
        strcpy(message.message_text, "ERROR_FILE_OPEN");
        msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
        return 1;
    }
    
    while (fgets(message.message_text, sizeof(message.message_text), file) != NULL)
    {
        msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
    }
    
    strcpy(message.message_text, "EOF");
    msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
    
    fclose(file);
    printf("Producer sent file data.\n");
    
    return 0;
}

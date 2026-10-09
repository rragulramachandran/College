#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

struct message_buffer
{
    long message_type;
    char message_text[1024];
};

int main()
{
    key_t key;
    int message_queue_id;
    pid_t pid;
    char src_filename[256];
    char dest_filename[256];
    
    printf("Enter source filename: ");
    scanf("%s", src_filename);
    printf("Enter destination filename: ");
    scanf("%s", dest_filename);
    
    key = ftok(".", 66);
    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    
    pid = fork();
    
    if (pid > 0)
    {
        struct message_buffer message;
        FILE *file;
        
        message.message_type = 1;
        
        if (access(src_filename, F_OK) == -1)
        {
            printf("Error: Source file does not exist.\n");
            strcpy(message.message_text, "ERROR_FILE_NOT_FOUND");
            msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
            wait(NULL);
            msgctl(message_queue_id, IPC_RMID, NULL);
            return 1;
        }
        
        if (access(src_filename, R_OK) == -1)
        {
            printf("Error: Source file is not readable.\n");
            strcpy(message.message_text, "ERROR_FILE_NOT_READABLE");
            msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
            wait(NULL);
            msgctl(message_queue_id, IPC_RMID, NULL);
            return 1;
        }
        
        file = fopen(src_filename, "r");
        if (file == NULL)
        {
            printf("Error: Cannot open source file.\n");
            strcpy(message.message_text, "ERROR_FILE_OPEN");
            msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
            wait(NULL);
            msgctl(message_queue_id, IPC_RMID, NULL);
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
        
        wait(NULL);
        msgctl(message_queue_id, IPC_RMID, NULL);
    }
    else if (pid == 0)
    {
        struct message_buffer message;
        FILE *file;
        
        file = fopen(dest_filename, "w");
        if (file == NULL)
        {
            printf("Consumer Error: Cannot create destination file.\n");
            return 1;
        }
        
        while (1)
        {
            msgrcv(message_queue_id, &message, sizeof(message.message_text), 1, 0);
            
            if (strcmp(message.message_text, "ERROR_FILE_NOT_FOUND") == 0 ||
                strcmp(message.message_text, "ERROR_FILE_NOT_READABLE") == 0 ||
                strcmp(message.message_text, "ERROR_FILE_OPEN") == 0)
            {
                fclose(file);
                remove(dest_filename);
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
    }
    
    return 0;
}

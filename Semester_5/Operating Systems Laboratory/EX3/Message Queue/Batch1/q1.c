#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
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
    
    key = ftok(".", 65);
    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    
    pid = fork();
    
    if (pid > 0)
    {
        struct message_buffer message;
        message.message_type = 1;
        
        printf("Enter message: ");
        fgets(message.message_text, sizeof(message.message_text), stdin);
        
        msgsnd(message_queue_id, &message, sizeof(message.message_text), 0);
        printf("Producer sent: %s", message.message_text);
        
        wait(NULL);
        msgctl(message_queue_id, IPC_RMID, NULL);
    }
    else if (pid == 0)
    {
        struct message_buffer message;
        int words = 0;
        int i = 0;
        int in_word = 0;
        
        msgrcv(message_queue_id, &message, sizeof(message.message_text), 1, 0);
        
        while (message.message_text[i] != '\0')
        {
            if (message.message_text[i] == ' ' || message.message_text[i] == '\n' || message.message_text[i] == '\t')
            {
                in_word = 0;
            }
            else if (in_word == 0)
            {
                in_word = 1;
                words++;
            }
            i++;
        }
        
        printf("Consumer calculated words: %d\n", words);
    }
    
    return 0;
}

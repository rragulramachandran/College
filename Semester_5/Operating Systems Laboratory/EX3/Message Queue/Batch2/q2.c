#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

struct student_marks
{
    int roll_no;
    char name[50];
    int mark1;
    int mark2;
    int mark3;
    int mark4;
    int mark5;
};

struct message_buffer
{
    long message_type;
    struct student_marks marks;
};

char get_grade(int mark)
{
    if (mark >= 90)
    {
        return 'S';
    }
    else if (mark >= 80)
    {
        return 'A';
    }
    else if (mark >= 70)
    {
        return 'B';
    }
    else if (mark >= 60)
    {
        return 'C';
    }
    else if (mark >= 50)
    {
        return 'D';
    }
    else
    {
        return 'F';
    }
}

int main()
{
    key_t key;
    int message_queue_id;
    pid_t pid;
    
    key = ftok(".", 67);
    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    
    pid = fork();
    
    if (pid > 0)
    {
        struct message_buffer message;
        message.message_type = 1;
        
        printf("Enter Student Roll No: ");
        scanf("%d", &message.marks.roll_no);
        printf("Enter Student Name: ");
        scanf("%s", message.marks.name);
        printf("Enter Mark 1: ");
        scanf("%d", &message.marks.mark1);
        printf("Enter Mark 2: ");
        scanf("%d", &message.marks.mark2);
        printf("Enter Mark 3: ");
        scanf("%d", &message.marks.mark3);
        printf("Enter Mark 4: ");
        scanf("%d", &message.marks.mark4);
        printf("Enter Mark 5: ");
        scanf("%d", &message.marks.mark5);
        
        msgsnd(message_queue_id, &message, sizeof(message.marks), 0);
        printf("Producer sent student marks.\n");
        
        wait(NULL);
        msgctl(message_queue_id, IPC_RMID, NULL);
    }
    else if (pid == 0)
    {
        struct message_buffer message;
        int total;
        float average;
        
        msgrcv(message_queue_id, &message, sizeof(message.marks), 1, 0);
        
        total = message.marks.mark1 + message.marks.mark2 + message.marks.mark3 + message.marks.mark4 + message.marks.mark5;
        average = total / 5.0;
        
        printf("\n--- Grade Sheet (Consumer) ---\n");
        printf("Roll No : %d\n", message.marks.roll_no);
        printf("Name    : %s\n", message.marks.name);
        printf("-------------------\n");
        printf("Subject 1 : %d (Grade: %c)\n", message.marks.mark1, get_grade(message.marks.mark1));
        printf("Subject 2 : %d (Grade: %c)\n", message.marks.mark2, get_grade(message.marks.mark2));
        printf("Subject 3 : %d (Grade: %c)\n", message.marks.mark3, get_grade(message.marks.mark3));
        printf("Subject 4 : %d (Grade: %c)\n", message.marks.mark4, get_grade(message.marks.mark4));
        printf("Subject 5 : %d (Grade: %c)\n", message.marks.mark5, get_grade(message.marks.mark5));
        printf("-------------------\n");
        printf("Total   : %d\n", total);
        printf("Average : %.2f\n", average);
    }
    
    return 0;
}

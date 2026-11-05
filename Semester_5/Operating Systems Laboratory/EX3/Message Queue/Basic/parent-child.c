
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct Message
{
    long type;
    int data;
};

int main()
{
    int msgid, msgk;
    struct Message msg;

    msgk = ftok(".", 65);
    msgid = msgget(msgk, IPC_CREAT | 0666);

    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid > 0)
    {
        while (1)
        {
            printf("Enter number (-1 to stop): ");
            fflush(stdout);
            scanf("%d", &msg.data);

            msg.type = 1;

            msgsnd(msgid, &msg, sizeof(msg.data), 0);

            if (msg.data == -1)
                break;
        }

        wait(NULL);
        msgctl(msgid, IPC_RMID, NULL);
    }
    else
    {
        while (1)
        {
            msgrcv(msgid, &msg, sizeof(msg.data), 1, 0);

            if (msg.data == -1)
                break;

            printf("[Child] Received: %d\n", msg.data);
        }
    }

    return 0;
}

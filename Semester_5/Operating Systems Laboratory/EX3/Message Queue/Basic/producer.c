#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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

    while (1)
    {
        printf("Enter number (-1 to stop): ");
        scanf("%d", &msg.data);

        msg.type = 1;

        msgsnd(msgid, &msg, sizeof(msg.data), 0);

        if (msg.data == -1)
            break;
    }

    return 0;
}

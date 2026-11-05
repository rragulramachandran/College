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
        msgrcv(msgid, &msg, sizeof(msg.data), 1, 0);

        if (msg.data == -1)
            break;

        printf("Received: %d\n", msg.data);
    }

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}

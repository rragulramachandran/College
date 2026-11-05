#include "head.h"

int main()
{
    key_t shmk, semk;

    shmk = ftok(".", 65);
    shmid = shmget(shmk, bufs * sizeof(int))
}
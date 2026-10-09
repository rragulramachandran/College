#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() 
{
    fork();
    fork();
    fork();
    
    printf("Hello\n");
    return 0;
}
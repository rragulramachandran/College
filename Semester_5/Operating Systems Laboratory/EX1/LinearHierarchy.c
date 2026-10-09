#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() 
{
  pid_t pid1, pid2;

  pid1 = fork();

  if (pid1 < 0) 
  {
    perror("Fork 1 failed");
    return 1;
  }

  if (pid1 == 0) 
  {
    pid2 = fork();

    if (pid2 < 0) 
    {
      perror("Fork 2 failed");
      return 1;
    }

    if (pid2 == 0) 
    {
      printf("[P3] Grandchild Process ID: %d, Parent (P2) ID: %d\n", getpid(),
             getppid());
      exit(0);
    } 
    else 
    {
      wait(NULL);
      printf("[P2] Child Process ID: %d, Parent (P1) ID: %d\n", getpid(),
             getppid());
      exit(0);
    }
  } 
  else 
  {
    wait(NULL);
    printf("[P1] Parent Process ID: %d\n", getpid());
  }

  return 0;
}

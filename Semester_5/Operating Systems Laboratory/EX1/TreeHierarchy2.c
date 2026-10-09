#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() 
{
  pid_t pid2, pid3, pid4, pid5, pid6, pid7;

  printf("=== Process Tree Execution Start ===\n\n");
  printf("[P1] Root Process ID: %d\n", getpid());
  fflush(stdout);

  pid2 = fork();

  if (pid2 < 0) 
  {
    perror("Fork for P2 failed");
    return 1;
  }

  if (pid2 == 0) 
  {
    usleep(100000);
    printf("[P2] Process ID: %d, Parent ID: %d\n", getpid(), getppid());
    fflush(stdout);

    pid4 = fork();

    if (pid4 < 0) 
    {
      perror("Fork for P4 failed");
      exit(1);
    }

    if (pid4 == 0) 
    {
      usleep(300000);
      printf("[P4] Process ID: %d, Parent ID: %d\n", getpid(), getppid());
      fflush(stdout);

      pid6 = fork();

      if (pid6 < 0) 
      {
        perror("Fork for P6 failed");
        exit(1);
      }

      if (pid6 == 0) 
      {
        usleep(600000);
        printf("[P6] Process ID: %d, Parent ID: %d\n", getpid(), getppid());
        fflush(stdout);
        exit(0);
      } 
      else 
      {
        wait(NULL);
        exit(0);
      }
    } 
    else 
    {
      wait(NULL);
      exit(0);
    }

  } 
  else 
  {
    pid3 = fork();

    if (pid3 < 0) 
    {
      perror("Fork for P3 failed");
      return 1;
    }

    if (pid3 == 0) 
    {
      usleep(200000);
      printf("[P3] Process ID: %d, Parent ID: %d\n", getpid(), getppid());
      fflush(stdout);

      pid5 = fork();

      if (pid5 < 0) 
      {
        perror("Fork for P5 failed");
        exit(1);
      }

      if (pid5 == 0) 
      {
        usleep(400000);
        printf("[P5] Process ID: %d, Parent ID: %d\n", getpid(), getppid());
        fflush(stdout);
        exit(0);
      }

      pid7 = fork();

      if (pid7 < 0) 
      {
        perror("Fork for P7 failed");
        exit(1);
      }

      if (pid7 == 0) 
      {
        usleep(500000);
        printf("[P7] Process ID: %d, Parent ID: %d\n", getpid(), getppid());
        fflush(stdout);
        exit(0);
      }

      wait(NULL);
      wait(NULL);
      exit(0);

    } 
    else 
    {
      wait(NULL);
      wait(NULL);

      printf("\n=== Process Tree Execution Complete ===\n");
    }
  }

  return 0;
}

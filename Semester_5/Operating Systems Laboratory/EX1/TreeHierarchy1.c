#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() 
{
  int arr[] = {1, 2, 3, 4, 5, 6};
  int n = sizeof(arr) / sizeof(arr[0]);
  pid_t pid2, pid3;

  pid2 = fork();

  if (pid2 < 0) 
  {
    perror("Fork for P2 failed");
    return 1;
  }

  if (pid2 == 0) 
  {
    int even_sum = 0;
    for (int i = 0; i < n; i++) 
    {
      if (arr[i] % 2 == 0) 
      {
        even_sum += arr[i];
      }
    }
    printf("[P2] Process ID: %d, Parent ID: %d\n", getpid(), getppid());
    printf("[P2] Sum of even elements: %d\n\n", even_sum);
    exit(0);
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
      int odd_sum = 0;
      for (int i = 0; i < n; i++) 
      {
        if (arr[i] % 2 != 0) 
        {
          odd_sum += arr[i];
        }
      }
      printf("[P3] Process ID: %d, Parent ID: %d\n", getpid(), getppid());
      printf("[P3] Sum of odd elements: %d\n\n", odd_sum);
      exit(0);
    } 
    else 
    {
      wait(NULL);
      wait(NULL);

      int total_sum = 0;
      for (int i = 0; i < n; i++) 
      {
        total_sum += arr[i];
      }
      printf("[P1] Process ID: %d\n", getpid());
      printf("[P1] Total sum of elements: %d\n", total_sum);
    }
  }

  return 0;
}

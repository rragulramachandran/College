#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int noOfProcess, noOfHoles;

    printf("Enter No. Of Process : ");
    scanf("%d", noOfProcess);

    printf("Enter No. Of Holes : ");
    scanf("%d", noOfHoles);

    int process[noOfProcess], holes[noOfHoles], allocated[noOfProcess],
        used[noOfHoles];

    for (int i = 0; i < noOfProcess; i++)
    {
        scanf("%d", &process[i]);
        allocated[i] = -1;
    }

    for (int i = 0; i < noOfHoles; i++)
    {
        scanf("%d", &holes[i]);
        used[i] = 0;
    }

    /* FIRST FIT */

    for (int i = 0; i < noOfProcess; i++)
    {
        int index = -1;
        for (int j = 0; j < noOfHoles; j++)
        {
            if ((used[j] != 1) && process[i] < holes[j])
            {
                index = j;
            }

            if (index != -1)
            {
                allocated[i] = index;
                used[index] = 1;
            }
        }
    }

    for (int i = 0; i < noOfProcess; i++)
    {
        int index = -1;
        for (int j = 0; j < noOfHoles; j++)
        {
            if ((used[j] != 1) && process[i] < holes[j])
            {
                index = j;
            }
        }
    }
}
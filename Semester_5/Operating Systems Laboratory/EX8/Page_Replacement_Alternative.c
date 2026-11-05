#include <stdio.h>

#define MAX 100

void display(int frames[], int f)
{
    int i;

    for (i = 0; i < f; i++)
    {
        if (frames[i] == -1)
            printf("- ");
        else
            printf("%d ", frames[i]);
    }
    printf("\n");
}

/* FIFO Page Replacement */
void FIFO(int pages[], int n, int f)
{
    int frames[MAX], i, j, k = 0;
    int pageFaults = 0, found;

    for (i = 0; i < f; i++)
        frames[i] = -1;

    printf("\nFIFO Page Replacement:\n");

    for (i = 0; i < n; i++)
    {
        found = 0;

        for (j = 0; j < f; j++)
        {
            if (frames[j] == pages[i])
            {
                found = 1;
                break;
            }
        }

        if (found == 0)
        {
            frames[k] = pages[i];
            k = (k + 1) % f;
            pageFaults++;
        }

        printf("%d -> ", pages[i]);
        display(frames, f);
    }

    printf("Total Page Faults = %d\n", pageFaults);
}

/* Optimal Page Replacement */
void Optimal(int pages[], int n, int f)
{
    int frames[MAX], i, j, k;
    int pageFaults = 0, found;
    int replace, farthest, nextUse;

    for (i = 0; i < f; i++)
        frames[i] = -1;

    printf("\nOptimal Page Replacement:\n");

    for (i = 0; i < n; i++)
    {
        found = 0;

        for (j = 0; j < f; j++)
        {
            if (frames[j] == pages[i])
            {
                found = 1;
                break;
            }
        }

        if (found == 0)
        {
            pageFaults++;

            /* Find an empty frame */
            replace = -1;

            for (j = 0; j < f; j++)
            {
                if (frames[j] == -1)
                {
                    replace = j;
                    break;
                }
            }

            /* If no empty frame, find page used farthest in future */
            if (replace == -1)
            {
                farthest = -1;

                for (j = 0; j < f; j++)
                {
                    nextUse = -1;

                    for (k = i + 1; k < n; k++)
                    {
                        if (pages[k] == frames[j])
                        {
                            nextUse = k;
                            break;
                        }
                    }

                    if (nextUse == -1)
                    {
                        replace = j;
                        break;
                    }

                    if (nextUse > farthest)
                    {
                        farthest = nextUse;
                        replace = j;
                    }
                }
            }

            frames[replace] = pages[i];
        }

        printf("%d -> ", pages[i]);
        display(frames, f);
    }

    printf("Total Page Faults = %d\n", pageFaults);
}

/* LRU Page Replacement */
void LRU(int pages[], int n, int f)
{
    int frames[MAX], recent[MAX];
    int i, j, k;
    int pageFaults = 0, found;
    int replace, least;

    for (i = 0; i < f; i++)
    {
        frames[i] = -1;
        recent[i] = -1;
    }

    printf("\nLRU Page Replacement:\n");

    for (i = 0; i < n; i++)
    {
        found = 0;

        for (j = 0; j < f; j++)
        {
            if (frames[j] == pages[i])
            {
                found = 1;
                recent[j] = i;
                break;
            }
        }

        if (found == 0)
        {
            pageFaults++;

            /* Find empty frame */
            replace = -1;

            for (j = 0; j < f; j++)
            {
                if (frames[j] == -1)
                {
                    replace = j;
                    break;
                }
            }

            /* Find least recently used page */
            if (replace == -1)
            {
                least = recent[0];
                replace = 0;

                for (k = 1; k < f; k++)
                {
                    if (recent[k] < least)
                    {
                        least = recent[k];
                        replace = k;
                    }
                }
            }

            frames[replace] = pages[i];
            recent[replace] = i;
        }

        printf("%d -> ", pages[i]);
        display(frames, f);
    }

    printf("Total Page Faults = %d\n", pageFaults);
}

int main()
{
    int pages[] = {
        1, 2, 3, 4, 2, 1, 5, 6, 2, 1,
        2, 3, 7, 6, 3, 2, 1, 2, 3, 6
    };

    int n = 20;
    int frames = 3;

    FIFO(pages, n, frames);
    Optimal(pages, n, frames);
    LRU(pages, n, frames);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

/* Prototypes (as given) */
int FIFO(int noOfFrames, int noOfPageRef, int *pageLst);
int LRU(int noOfFrames, int noOfPageRef, int *pageLst);
int OPTIMAL(int noOfFrames, int noOfPageRef, int *pageLst);

/* ---------- Helper: print the current state of the frames ---------- */
void printFrames(int *frames, int n, int page, int isFault)
{
    printf("Page %2d  ->  ", page);
    for (int i = 0; i < n; i++)
    {
        if (frames[i] == -1)
            printf(" - ");
        else
            printf("%2d ", frames[i]);
    }
    printf("  %s\n", isFault ? "[FAULT]" : "[HIT]");
}

/* ---------- Helper: is 'page' already in a frame? Returns index or -1
 * ---------- */
int findPage(int *frames, int n, int page)
{
    for (int i = 0; i < n; i++)
        if (frames[i] == page)
            return i;
    return -1;
}

/* ============================ FIFO ============================ */
int FIFO(int noOfFrames, int noOfPageRef, int *pageLst)
{
    int *frames = (int *)malloc(noOfFrames * sizeof(int));
    int faults = 0;
    int next = 0; /* index of the oldest page (next victim) */

    for (int i = 0; i < noOfFrames; i++)
        frames[i] = -1; /* -1 means the frame is empty */

    for (int i = 0; i < noOfPageRef; i++)
    {
        int page = pageLst[i];
        int isFault = 0;

        if (findPage(frames, noOfFrames, page) == -1)
        {                                   /* page miss */
            frames[next] = page;            /* replace oldest */
            next = (next + 1) % noOfFrames; /* move pointer circularly */
            faults++;
            isFault = 1;
        }
        printFrames(frames, noOfFrames, page, isFault);
    }

    free(frames);
    return faults;
}

/* ============================ LRU ============================= */
int LRU(int noOfFrames, int noOfPageRef, int *pageLst)
{
    int *frames = (int *)malloc(noOfFrames * sizeof(int));
    int *lastUsed =
        (int *)malloc(noOfFrames * sizeof(int)); /* time of last use */
    int faults = 0;

    for (int i = 0; i < noOfFrames; i++)
    {
        frames[i] = -1;
        lastUsed[i] = -1;
    }

    for (int i = 0; i < noOfPageRef; i++)
    {
        int page = pageLst[i];
        int isFault = 0;
        int pos = findPage(frames, noOfFrames, page);

        if (pos != -1)
        {
            lastUsed[pos] = i; /* HIT: just refresh its usage time */
        }
        else
        { /* MISS */
            int victim = 0;
            int emptyFound = 0;

            /* 1) use an empty frame if there is one */
            for (int j = 0; j < noOfFrames; j++)
            {
                if (frames[j] == -1)
                {
                    victim = j;
                    emptyFound = 1;
                    break;
                }
            }
            /* 2) otherwise pick the frame with the smallest lastUsed time */
            if (!emptyFound)
            {
                for (int j = 1; j < noOfFrames; j++)
                    if (lastUsed[j] < lastUsed[victim])
                        victim = j;
            }

            frames[victim] = page;
            lastUsed[victim] = i;
            faults++;
            isFault = 1;
        }
        printFrames(frames, noOfFrames, page, isFault);
    }

    free(frames);
    free(lastUsed);
    return faults;
}

/* ========================== OPTIMAL =========================== */
int OPTIMAL(int noOfFrames, int noOfPageRef, int *pageLst)
{
    int *frames = (int *)malloc(noOfFrames * sizeof(int));
    int faults = 0;

    for (int i = 0; i < noOfFrames; i++)
        frames[i] = -1;

    for (int i = 0; i < noOfPageRef; i++)
    {
        int page = pageLst[i];
        int isFault = 0;

        if (findPage(frames, noOfFrames, page) == -1)
        { /* MISS */
            int victim = -1;

            /* 1) use an empty frame if there is one */
            for (int j = 0; j < noOfFrames; j++)
            {
                if (frames[j] == -1)
                {
                    victim = j;
                    break;
                }
            }

            /* 2) otherwise evict the page used farthest in the future */
            if (victim == -1)
            {
                int farthest = -1;
                for (int j = 0; j < noOfFrames; j++)
                {
                    int nextUse = -1;
                    for (int k = i + 1; k < noOfPageRef; k++)
                    {
                        if (pageLst[k] == frames[j])
                        {
                            nextUse = k;
                            break;
                        }
                    }
                    if (nextUse == -1)
                    { /* never used again: perfect victim */
                        victim = j;
                        break;
                    }
                    if (nextUse > farthest)
                    {
                        farthest = nextUse;
                        victim = j;
                    }
                }
            }

            frames[victim] = page;
            faults++;
            isFault = 1;
        }
        printFrames(frames, noOfFrames, page, isFault);
    }

    free(frames);
    return faults;
}

/* ============================ MAIN ============================ */
int main(void)
{
    int noOfFrames, noOfPageRef;

    printf("Enter number of frames: ");
    scanf("%d", &noOfFrames);

    printf("Enter number of page references: ");
    scanf("%d", &noOfPageRef);

    if (noOfFrames <= 0 || noOfPageRef <= 0)
    {
        printf("Frames and references must be positive.\n");
        return 1;
    }

    int *pageLst = (int *)malloc(noOfPageRef * sizeof(int));
    printf("Enter the page reference string: ");
    for (int i = 0; i < noOfPageRef; i++)
        scanf("%d", &pageLst[i]);

    printf("\n---------- FIFO ----------\n");
    int fifoFaults = FIFO(noOfFrames, noOfPageRef, pageLst);

    printf("\n---------- LRU ----------\n");
    int lruFaults = LRU(noOfFrames, noOfPageRef, pageLst);

    printf("\n---------- OPTIMAL ----------\n");
    int optFaults = OPTIMAL(noOfFrames, noOfPageRef, pageLst);

    printf("\n======== SUMMARY ========\n");
    printf("FIFO    page faults : %d\n", fifoFaults);
    printf("LRU     page faults : %d\n", lruFaults);
    printf("OPTIMAL page faults : %d\n", optFaults);

    free(pageLst);
    return 0;
}
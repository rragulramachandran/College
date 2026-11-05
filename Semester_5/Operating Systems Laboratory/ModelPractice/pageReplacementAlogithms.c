#include<stdio.h>
#include<stdlib.h>

int FIFO(int noOfFrames, int noOfPageRef, int *pageLst);
int LRU(int noOfFrames, int noOfPageRef, int *pageLst);
int OPTIMAL(int noOfFrames, int noOfPageRef, int *pageLst);

void printFrames(int * frames,int n, int page, int isFault)
{

}

int findPage(int * frames,int n, int page)
{
    for(int i = 0; i < n; i++)
    {
        if(frames[i] == page)
        {
            return i;
        }
        return -1;
    }
}

int FIFO(int noOfFrames, int noOfPageRef, int * pageLst)
{
    int * frames = (int *)malloc(noOfFrames * sizeof(int));
    int faults = 0;
    int next = 0;

    for(int i = 0; i < noOfFrames; i++)
    {
        frames[i] = -1;
    }

    for(int i = 0; i < noOfPageRef; i++)
    {
        int page = pageLst[i];
        int isFault = 0;

        if(findPage(frames, noOfFrames, page) == -1)
        {
            frames[next] = page;
            next = (next + 1) % noOfFrames;
            faults++;
            isFault = 1;
        }
        printFrames(frames, noOfFrames, page, isFault);
    }

    free(frames);
    return faults;
}

int LRU(int noOfFrames, int noOfPageRef, int * pageLst)
{
    int * frames = (int *)malloc(noOfFrames * sizeof(int));
    int * lastUsed = (int *)malloc(noOfFrames * sizeof(int));
    int faults = 0;

    for(int i = 0; i < noOfFrames; i++)
    {
        frames[i] = -1;
        lastUsed[i] = -1;
    }

    for(int i = 0; i < noOfPageRef; i++)
    {
        int page = pageLst[i];
        int isFault = 0;
        int pos = findPage(noOfFrames, noOfPageRef, page);

        if(pos != -1)
        {
            lastUsed[pos] = i;
        }
        else
        {
            int victim = 0, emptyFound = 0;

            for(int j = 0; j < noOfFrames; j++)
            {
                if(frames[j] == -1)
                {
                    victim = j;
                    emptyFound = 1;
                    break;
                }
                
                if(!emptyFound)
                {
                    for(int j = 0; j < noOfFrames; j++)
                    {
                        if(lastUsed[i] < lastUsed[victim])
                        {
                            victim = i;
                        }
                    }
                }

                frames[victim] = page;
                lastUsed[victim] = j;
                faults++;
                isFault = 1;
            }
            printFrames(frames, noOfFrames, page, isFault);
        }

    }
    free(frames);
    free(lastUsed);
    return faults;
}
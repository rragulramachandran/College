#include <stdio.h>
#include <string.h>
#include "head.h"

int disk[SIZE];
char name[MAX][20];
char content[MAX][100];
int start[MAX], length[MAX]; 
int files = 0;

int findFile(char fname[])
{
    int i;

    for (i = 0; i < files; i++)
    {
        if (strcmp(name[i], fname) == 0)
            return i;
    }

    return -1;
}

void displayDirectory()
{
    int i;

    if (files == 0)
    {
        printf("\nDirectory is empty!\n");
        return;
    }

    printf("\nFile Directory:\n");
    printf("File Name\tStart\tLength\n");

    for (i = 0; i < files; i++)
        printf("%s\t\t%d\t%d\n",
               name[i], start[i], length[i]);
}

void displayContent()
{
    char fname[20];
    int pos;

    printf("Enter file name: ");
    scanf("%19s", fname);

    pos = findFile(fname);

    if (pos == -1)
        printf("File not found!\n");
    else
        printf("Content: %s\n", content[pos]);
}

void insertFile()
{
    int len, i, j, startBlock = -1;
    char fname[20];

    if (files == MAX)
    {
        printf("Directory is full!\n");
        return;
    }

    printf("Enter file name: ");
    scanf("%19s", fname);

    if (findFile(fname) != -1)
    {
        printf("File already exists!\n");
        return;
    }

    printf("Enter file length: ");
    scanf("%d", &len);

    if (len <= 0 || len > SIZE)
    {
        printf("Invalid file length!\n");
        return;
    }

    for (i = 0; i <= SIZE - len; i++)
    {
        for (j = i; j < i + len; j++)
        {
            if (disk[j] == 0)
                break;
        }

        if (j == i + len)
        {
            startBlock = i;
            break;
        }
    }

    if (startBlock == -1)
    {
        printf("Allocation failed! No consecutive free blocks available.\n");
        return;
    }

    for (i = startBlock; i < startBlock + len; i++)
        disk[i] = 0;

    start[files] = startBlock;
    length[files] = len;

    printf("Enter file content: ");
    getchar();
    fgets(content[files], 100, stdin);

    content[files][strcspn(content[files], "\n")] = '\0';

    strcpy(name[files], fname);
    files++;

    printf("File inserted successfully!\n");
    printf("Starting block: %d\n", startBlock);
    printf("Ending block: %d\n", startBlock + len - 1);
}

void searchFile()
{
    char fname[20];
    int pos;

    printf("Enter file name to search: ");
    scanf("%19s", fname);

    pos = findFile(fname);

    if (pos == -1)
        printf("File not found!\n");
    else
    {
        printf("File found!\n");
        printf("Name: %s\n", name[pos]);
        printf("Starting block: %d\n", start[pos]);
        printf("Ending block: %d\n",
               start[pos] + length[pos] - 1);
        printf("Length: %d\n", length[pos]);
    }
}

void deleteFile()
{
    char fname[20];
    int pos, i;

    printf("Enter file name to delete: ");
    scanf("%19s", fname);

    pos = findFile(fname);

    if (pos == -1)
    {
        printf("File not found!\n");
        return;
    }

    for (i = start[pos]; i < start[pos] + length[pos]; i++)
        disk[i] = 1;

    for (i = pos; i < files - 1; i++)
    {
        strcpy(name[i], name[i + 1]);
        strcpy(content[i], content[i + 1]);
        start[i] = start[i + 1];
        length[i] = length[i + 1];
    }

    files--;

    printf("File deleted successfully!\n");
}

void displayDisk()
{
    int i;

    printf("\nDisk Blocks:\n");

    for (i = 0; i < SIZE; i++)
        printf("%d:%s  ", i,
               disk[i] == 1 ? "Free" : "Used");

    printf("\n");
}

void displayAllFiles()
{
    int i;

    if (files == 0)
    {
        printf("\nNo files available!\n");
        return;
    }

    printf("\n--- All File Details ---\n");

    for (i = 0; i < files; i++)
    {
        printf("\nFile Name: %s\n", name[i]);
        printf("Content: %s\n", content[i]);
        printf("Starting Block: %d\n", start[i]);
        printf("Ending Block: %d\n",
               start[i] + length[i] - 1);
        printf("Length: %d\n", length[i]);
    }
}


#include <stdio.h>
#include "head.h"

int main()
{
    int choice, i;

    printf("Enter Bit Vector for 20 blocks (1 = Free, 0 = Used):\n");

    for (i = 0; i < SIZE; i++)
    {
        scanf("%d", &disk[i]);

        if (disk[i] != 0 && disk[i] != 1)
        {
            printf("Invalid bit! Enter only 0 or 1.\n");
            return 1;
        }
    }

    while (1)
    {
        printf("\n--- Contiguous File Allocation ---\n");
        printf("1. Display Files (Directory)\n");
        printf("2. Display Content\n");
        printf("3. Insert File\n");
        printf("4. Search File\n");
        printf("5. Delete File\n");
        printf("6. Display Disk Blocks\n");
        printf("7. Display All File Details\n");
        printf("0. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                displayDirectory();
                break;

            case 2:
                displayContent();
                break;

            case 3:
                insertFile();
                break;

            case 4:
                searchFile();
                break;

            case 5:
                deleteFile();
                break;

            case 6:
                displayDisk();
                break;

            case 7:
                displayAllFiles();
                break;

            case 0:
                return 0;

            default:
                printf("Invalid choice!\n");
        }
    }

    return 0;
}

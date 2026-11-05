#include <stdio.h>
#include <string.h>
#include "head.h"

int main() {
    initDisk();
    int choice;
    char name[30], content[100];
    int length;

    printf("=======================================================\n");
    printf("   Contiguous File Allocation System (Bit Vector)      \n");
    printf("=======================================================\n");

    do {
        printf("\n----------------------- MENU -----------------------\n");
        printf("1. Insert File (Contiguous First-Fit Allocation)\n");
        printf("2. Display Directory\n");
        printf("3. Display File Content\n");
        printf("4. Search File\n");
        printf("5. Delete File\n");
        printf("6. Display Disk Blocks (Bit Vector)\n");
        printf("7. Display All Files (Metadata + Content)\n");
        printf("8. Exit\n");
        printf("----------------------------------------------------\n");
        printf("Enter your choice (1-8): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter File Name (e.g., file1.txt): ");
                scanf("%29s", name);
                printf("Enter File Length in blocks: ");
                scanf("%d", &length);
                getchar(); // Consume trailing newline
                printf("Enter File Content: ");
                fgets(content, sizeof(content), stdin);
                content[strcspn(content, "\n")] = '\0';
                insertFile(name, length, content);
                break;

            case 2:
                displayDirectory();
                break;

            case 3:
                printf("Enter File Name to display content: ");
                scanf("%29s", name);
                displayFileContent(name);
                break;

            case 4:
                printf("Enter File Name to search: ");
                scanf("%29s", name);
                searchFile(name);
                break;

            case 5:
                printf("Enter File Name to delete: ");
                scanf("%29s", name);
                deleteFile(name);
                break;

            case 6:
                displayDiskBlocks();
                break;

            case 7:
                displayAllFiles();
                break;

            case 8:
                printf("Exiting File Allocation Simulator. Good luck!\n");
                break;

            default:
                printf("Invalid choice! Please select 1 to 8.\n");
        }
    } while (choice != 8);

    return 0;
}

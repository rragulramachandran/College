#include <stdio.h>
#include <string.h>
#include "head.h"

int bitVector[SIZE];
File directory[MAX];
int fileCount = 0;

// Initialize all blocks as Free (1)
void initDisk() {
    for (int i = 0; i < SIZE; i++) {
        bitVector[i] = 1; // 1 = Free
    }
    fileCount = 0;
}

// 1. Insert File using First-Fit Contiguous Allocation
int insertFile(const char *name, int length, const char *content) {
    if (fileCount >= MAX) {
        printf("[ERROR] Directory is full! Cannot add more files.\n");
        return 0;
    }

    // Check for duplicate file name
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(directory[i].name, name) == 0) {
            printf("[ERROR] File with name '%s' already exists!\n", name);
            return 0;
        }
    }

    if (length <= 0 || length > SIZE) {
        printf("[ERROR] Invalid file length!\n");
        return 0;
    }

    // First-Fit search for contiguous free blocks
    int start = -1;
    for (int i = 0; i <= SIZE - length; i++) {
        int contiguous = 1;
        for (int j = 0; j < length; j++) {
            if (bitVector[i + j] == 0) { // Block occupied
                contiguous = 0;
                break;
            }
        }
        if (contiguous) {
            start = i;
            break; // First-Fit: pick the first matching contiguous segment
        }
    }

    if (start == -1) {
        printf("[ERROR] Allocation Failed: Not enough contiguous free blocks available for '%s'!\n", name);
        return 0;
    }

    // Mark allocated blocks as Used (0) in the Bit Vector
    for (int j = 0; j < length; j++) {
        bitVector[start + j] = 0;
    }

    // Add entry to the directory
    strcpy(directory[fileCount].name, name);
    directory[fileCount].start = start;
    directory[fileCount].length = length;
    strcpy(directory[fileCount].content, content);
    fileCount++;

    printf("[SUCCESS] File '%s' allocated from block %d to %d (Length: %d).\n", 
           name, start, start + length - 1, length);
    return 1;
}

// 2. Display Directory (File Allocation Table)
void displayDirectory() {
    if (fileCount == 0) {
        printf("[INFO] Directory is empty.\n");
        return;
    }
    printf("\n===================================================\n");
    printf("              FILE ALLOCATION TABLE                \n");
    printf("===================================================\n");
    printf("| %-15s | %-13s | %-11s |\n", "File Name", "Start Block", "Length");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < fileCount; i++) {
        printf("| %-15s | %-13d | %-11d |\n", 
               directory[i].name, directory[i].start, directory[i].length);
    }
    printf("===================================================\n");
}

// 3. Display File Content
void displayFileContent(const char *name) {
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(directory[i].name, name) == 0) {
            printf("\n--- File Content for '%s' ---\n", name);
            printf("%s\n", directory[i].content);
            printf("--------------------------------\n");
            return;
        }
    }
    printf("[ERROR] File '%s' not found!\n", name);
}

// 4. Search File Metadata
void searchFile(const char *name) {
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(directory[i].name, name) == 0) {
            printf("\n[FILE FOUND]\n");
            printf("  File Name   : %s\n", directory[i].name);
            printf("  Start Block : %d\n", directory[i].start);
            printf("  End Block   : %d\n", directory[i].start + directory[i].length - 1);
            printf("  Length      : %d blocks\n", directory[i].length);
            return;
        }
    }
    printf("[ERROR] File '%s' does not exist in directory!\n", name);
}

// 5. Delete File and Free Blocks to Bit Vector
int deleteFile(const char *name) {
    int idx = -1;
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(directory[i].name, name) == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("[ERROR] File '%s' not found for deletion!\n", name);
        return 0;
    }

    // Free the blocks in Bit Vector (set back to 1)
    int start = directory[idx].start;
    int length = directory[idx].length;
    for (int j = 0; j < length; j++) {
        bitVector[start + j] = 1; // 1 = Free
    }

    // Shift directory entries to close the gap
    for (int i = idx; i < fileCount - 1; i++) {
        directory[i] = directory[i + 1];
    }
    fileCount--;

    printf("[SUCCESS] File '%s' deleted. Blocks %d to %d are now FREE.\n", 
           name, start, start + length - 1);
    return 1;
}

// 6. Display Disk Blocks Allocation Status
void displayDiskBlocks() {
    printf("\n============================================================\n");
    printf("                  DISK BIT VECTOR (SIZE: %d)                \n", SIZE);
    printf("               (1 = FREE [F],  0 = USED [U])                \n");
    printf("============================================================\n");
    printf("Block  : ");
    for (int i = 0; i < SIZE; i++) printf("%2d ", i);
    printf("\nStatus : ");
    for (int i = 0; i < SIZE; i++) {
        printf(" %c ", bitVector[i] ? 'F' : 'U');
    }
    printf("\n============================================================\n");
}

// 7. Display All Resident Files with Complete Details
void displayAllFiles() {
    if (fileCount == 0) {
        printf("[INFO] No resident files on disk.\n");
        return;
    }
    printf("\n=========================================================================\n");
    printf("                         ALL RESIDENT FILES                              \n");
    printf("=========================================================================\n");
    for (int i = 0; i < fileCount; i++) {
        printf("File #%d:\n", i + 1);
        printf("  Name       : %s\n", directory[i].name);
        printf("  Allocation : Blocks %d to %d (Length: %d)\n", 
               directory[i].start, directory[i].start + directory[i].length - 1, directory[i].length);
        printf("  Content    : \"%s\"\n\n", directory[i].content);
    }
    printf("=========================================================================\n");
}

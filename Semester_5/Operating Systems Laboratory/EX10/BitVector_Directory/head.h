#ifndef HEAD_H
#define HEAD_H

#define SIZE 20 // Fixed disk size: 20 blocks
#define MAX  10 // Maximum directory capacity: 10 files

// File metadata structure
typedef struct {
    char name[30];
    int start;
    int length;
    char content[100];
} File;

// Global disk and directory declarations
extern int bitVector[SIZE]; // 1 = Free, 0 = Used
extern File directory[MAX];
extern int fileCount;

// Function prototypes
void initDisk();
int insertFile(const char *name, int length, const char *content);
void displayDirectory();
void displayFileContent(const char *name);
void searchFile(const char *name);
int deleteFile(const char *name);
void displayDiskBlocks();
void displayAllFiles();

#endif

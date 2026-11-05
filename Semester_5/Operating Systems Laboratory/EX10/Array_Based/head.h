#ifndef HEAD_H
#define HEAD_H

#define SIZE 20
#define MAX 10

extern int disk[SIZE];
extern char name[MAX][20];
extern char content[MAX][100];
extern int start[MAX], length[MAX];
extern int files;

void displayDirectory();
void displayContent();
void insertFile();
void searchFile();
void deleteFile();
void displayDisk();
void displayAllFiles();

#endif
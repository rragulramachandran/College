#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define CHUNK_SIZE 1024
#define DEFAULT_SHARED_FILE "shared_file.txt"

// Semaphore indices in the semaphore set
#define SEM_MUTEX 0  // Binary Semaphore (mutual exclusion)
#define SEM_EMPTY 1  // Counting Semaphore (empty slots)
#define SEM_FULL  2  // Counting Semaphore (filled slots)

// Synchronization Modes
#define MODE_SHARED_FILE    1  // Producer: File Write -> Consumer: File Read (Shared File)
#define MODE_FILE_TRANSFER  2  // Producer: File Read  -> Buffer -> Consumer: File Write
#define MODE_INTERACTIVE    3  // Interactive Character Stream (Classic)

// Chunk item structure for file transfer mode
typedef struct {
    char data[CHUNK_SIZE];
    int size;       // Number of bytes in this chunk
    int is_eof;     // 1 if this is the EOF chunk, 0 otherwise
} BufferChunk;

// Circular buffer structure residing in shared memory
typedef struct {
    int mode;                      // Mode selected by Producer (1, 2, or 3)
    int in;                        // Write index
    int out;                       // Read index
    int total_items;               // Counter of items transferred
    int is_finished;               // 1 when Producer has finished
    char shared_filename[256];     // Filename for Mode 1 (Shared file)
    char source_filename[256];     // Filename for Mode 2 (Source file)
    char dest_filename[256];       // Filename for Mode 2 (Destination file)

    // Data storage for bounded buffer
    char char_data[BUFFER_SIZE];          // For Mode 3 (Character stream)
    BufferChunk chunk_data[BUFFER_SIZE];  // For Mode 2 (File transfer)
} SharedBuffer;

// System V Semaphore union definition
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
#endif

// P operation (wait / decrement)
static inline void sem_wait_op(int semid, int sem_num) {
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = -1; // Decrement
    op.sem_flg = 0;
    semop(semid, &op, 1);
}

// V operation (signal / increment)
static inline void sem_signal_op(int semid, int sem_num) {
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = 1;  // Increment
    op.sem_flg = 0;
    semop(semid, &op, 1);
}

// Helper to get semaphore value for diagnostics
static inline int sem_get_val(int semid, int sem_num) {
    return semctl(semid, sem_num, GETVAL);
}

#endif

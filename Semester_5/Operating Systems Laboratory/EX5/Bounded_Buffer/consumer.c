#include "buffer.h"

int main() {
    // Step 1: Derive the same IPC keys
    key_t shm_key = ftok(".", 'S');
    key_t sem_key = ftok(".", 'M');

    if (shm_key == -1 || sem_key == -1) {
        perror("ftok failed");
        exit(1);
    }

    // Step 2: Access existing shared memory segment
    int shmid = shmget(shm_key, sizeof(SharedBuffer), 0666);
    if (shmid < 0) {
        perror("shmget failed (Make sure Producer is running first)");
        exit(1);
    }
    SharedBuffer *buffer = (SharedBuffer *)shmat(shmid, NULL, 0);
    if (buffer == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Step 3: Access existing semaphore set
    int semid = semget(sem_key, 3, 0666);
    if (semid < 0) {
        perror("semget failed (Make sure Producer is running first)");
        shmdt(buffer);
        exit(1);
    }

    // Step 4: Wait until Producer selects a mode
    if (buffer->mode == 0) {
        printf("[Consumer] Waiting for Producer to select an operation mode...\n");
        while (buffer->mode == 0) {
            usleep(200000); // Poll every 200ms
        }
    }

    printf("==================================================================\n");
    printf("  [Consumer] Process Synchronization (Producer-Consumer Problem)\n");
    printf("  Connected to Shared Memory (ID: %d) & Semaphores (ID: %d)\n", shmid, semid);
    if (buffer->mode == MODE_SHARED_FILE) {
        printf("  Active Mode: [1] Shared File Synchronization (File Read)\n");
    } else if (buffer->mode == MODE_FILE_TRANSFER) {
        printf("  Active Mode: [2] Synchronized File Transfer (File Write)\n");
    } else {
        printf("  Active Mode: [3] Classic Character Stream\n");
    }
    printf("==================================================================\n\n");

    // -------------------------------------------------------------------------
    // MODE 1: Shared File Synchronization
    // Consumer reads records from the shared file (File Read).
    // Mutex guarantees exclusive file read access; counting semaphores synchronize.
    // -------------------------------------------------------------------------
    if (buffer->mode == MODE_SHARED_FILE) {
        printf("[Consumer] Monitoring shared file: '%s'\n", buffer->shared_filename);
        printf("[Consumer] Waiting for Producer to write data...\n\n");

        long read_offset = 0;
        int items_consumed = 0;

        while (1) {
            // Classical Synchronization:
            sem_wait_op(semid, SEM_FULL);  // Wait until Producer signals that data is in file
            sem_wait_op(semid, SEM_MUTEX); // Acquire exclusive file access lock

            // --- CRITICAL SECTION: FILE READ ---
            FILE *fp = fopen(buffer->shared_filename, "r");
            char line[512] = {0};

            if (fp) {
                fseek(fp, read_offset, SEEK_SET);
                if (fgets(line, sizeof(line), fp) != NULL) {
                    read_offset = ftell(fp);
                }
                fclose(fp);
            }

            int slot = buffer->out;
            buffer->out = (buffer->out + 1) % BUFFER_SIZE;

            // Remove newline
            line[strcspn(line, "\r\n")] = '\0';
            int is_eof = (strcmp(line, "__EOF__") == 0);

            if (!is_eof) {
                items_consumed++;
                printf(" <- [Consumed - File Read] Read from '%s' (Slot [%d]): \"%s\"\n",
                       buffer->shared_filename, slot, line);
                printf("    [Semaphores] SEM_EMPTY: %d | SEM_FULL: %d\n",
                       sem_get_val(semid, SEM_EMPTY) + 1, sem_get_val(semid, SEM_FULL));
            }

            sem_signal_op(semid, SEM_MUTEX); // Release file lock
            sem_signal_op(semid, SEM_EMPTY); // Signal Producer that space is freed in buffer

            if (is_eof) {
                printf("\n[Consumer] EOF marker received from '%s'.\n", buffer->shared_filename);
                printf("[Consumer] Total items consumed: %d\n", items_consumed);
                break;
            }

            // Visual delay to showcase synchronization
            usleep(300000);
        }
    }
    // -------------------------------------------------------------------------
    // MODE 2: Synchronized File Transfer (Read from buffer -> Write to Destination)
    // Consumer takes chunks from bounded buffer and writes to destination file (File Write).
    // -------------------------------------------------------------------------
    else if (buffer->mode == MODE_FILE_TRANSFER) {
        printf("[Consumer] Preparing destination file: '%s'\n", buffer->dest_filename);
        FILE *fp_out = fopen(buffer->dest_filename, "wb");
        if (!fp_out) {
            perror("Failed to open destination file");
            shmdt(buffer);
            exit(1);
        }

        printf("[Consumer] Streaming chunks from bounded buffer to '%s'...\n\n", buffer->dest_filename);

        int chunks_consumed = 0;
        long total_bytes = 0;

        while (1) {
            sem_wait_op(semid, SEM_FULL);  // Wait for available chunk in buffer
            sem_wait_op(semid, SEM_MUTEX); // Enter Critical Section

            // --- CRITICAL SECTION: RETRIEVE CHUNK FROM BUFFER ---
            int slot = buffer->out;
            int size = buffer->chunk_data[slot].size;
            int is_eof = buffer->chunk_data[slot].is_eof;

            char temp_chunk[CHUNK_SIZE];
            if (size > 0) {
                memcpy(temp_chunk, buffer->chunk_data[slot].data, size);
            }
            buffer->out = (buffer->out + 1) % BUFFER_SIZE;

            sem_signal_op(semid, SEM_MUTEX); // Exit Critical Section
            sem_signal_op(semid, SEM_EMPTY); // Signal Producer that slot is available

            // --- FILE WRITE OPERATION ---
            if (size > 0) {
                fwrite(temp_chunk, 1, size, fp_out);
                total_bytes += size;
            }
            chunks_consumed++;

            printf(" <- [Consumed - File Write] Chunk #%d (%d bytes) from Buffer slot [%d] -> '%s' (EOF=%d)\n",
                   chunks_consumed, size, slot, buffer->dest_filename, is_eof);

            if (is_eof) {
                printf("\n[Consumer] EOF signal received. Total bytes written: %ld\n", total_bytes);
                break;
            }

            usleep(200000);
        }

        fclose(fp_out);
        printf("[Consumer] Destination file '%s' saved and closed successfully.\n", buffer->dest_filename);
    }
    // -------------------------------------------------------------------------
    // MODE 3: Classic Interactive Character Stream
    // -------------------------------------------------------------------------
    else {
        printf("[Consumer] Starting character stream consumption...\n\n");
        int out = 0;
        while (1) {
            sem_wait_op(semid, SEM_FULL);
            sem_wait_op(semid, SEM_MUTEX);

            char ch = buffer->char_data[out];
            int slot = out;
            out = (out + 1) % BUFFER_SIZE;

            sem_signal_op(semid, SEM_MUTEX);
            sem_signal_op(semid, SEM_EMPTY);

            if (ch == 'q') {
                printf("\n[Consumer] Received 'q' (Quit signal). Exiting loop.\n");
                break;
            }

            printf(" <- [Consumed] '%c' (ASCII: %3d) retrieved from slot [%d]\n", ch, (int)ch, slot);
            sleep(1);
        }
    }

    // Step 5: Clean up IPC resources (Shared Memory & Semaphores)
    shmdt(buffer);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    printf("[Consumer] Shared memory and Semaphores cleaned up from kernel.\n");
    printf("[Consumer] Terminating successfully.\n");

    return 0;
}

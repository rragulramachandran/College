#include "buffer.h"

// Helper function to create a sample source file if needed for Mode 2
static void create_sample_file(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "Line 1: Operating Systems Lab - Process Synchronization\n");
        fprintf(fp, "Line 2: Classic Producer-Consumer Bounded Buffer Problem\n");
        fprintf(fp, "Line 3: Synchronized file transfer using System V Semaphores\n");
        fprintf(fp, "Line 4: Semaphores: SEM_MUTEX (1), SEM_EMPTY (5), SEM_FULL (0)\n");
        fprintf(fp, "Line 5: End of Sample Transmission.\n");
        fclose(fp);
        printf("[Producer] Created sample input file: '%s'\n", filename);
    }
}

int main() {
    // Step 1: Generate unique IPC keys using ftok
    key_t shm_key = ftok(".", 'S');
    key_t sem_key = ftok(".", 'M');

    if (shm_key == -1 || sem_key == -1) {
        perror("ftok failed");
        exit(1);
    }

    // Step 2: Create and attach shared memory segment
    int shmid = shmget(shm_key, sizeof(SharedBuffer), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    SharedBuffer *buffer = (SharedBuffer *)shmat(shmid, NULL, 0);
    if (buffer == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Initialize shared buffer state
    buffer->mode = 0;
    buffer->in = 0;
    buffer->out = 0;
    buffer->total_items = 0;
    buffer->is_finished = 0;
    memset(buffer->shared_filename, 0, sizeof(buffer->shared_filename));
    memset(buffer->source_filename, 0, sizeof(buffer->source_filename));
    memset(buffer->dest_filename, 0, sizeof(buffer->dest_filename));

    // Step 3: Create a semaphore set of 3 semaphores
    // [0] SEM_MUTEX: Binary semaphore (Mutual Exclusion)
    // [1] SEM_EMPTY: Counting semaphore (Empty slots tracker)
    // [2] SEM_FULL:  Counting semaphore (Filled slots tracker)
    int semid = semget(sem_key, 3, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget failed");
        exit(1);
    }

    // Step 4: Initialize semaphores
    union semun arg;
    arg.val = 1;            // SEM_MUTEX = 1 (unlocked)
    semctl(semid, SEM_MUTEX, SETVAL, arg);

    arg.val = BUFFER_SIZE;  // SEM_EMPTY = 5 (all slots free)
    semctl(semid, SEM_EMPTY, SETVAL, arg);

    arg.val = 0;            // SEM_FULL = 0 (no filled items initially)
    semctl(semid, SEM_FULL, SETVAL, arg);

    printf("==================================================================\n");
    printf("  [Producer] Process Synchronization (Producer-Consumer Problem)\n");
    printf("  Shared Memory ID: %d | Semaphore Set ID: %d\n", shmid, semid);
    printf("  Buffer Capacity : %d | SEM_MUTEX: 1 | SEM_EMPTY: %d | SEM_FULL: 0\n", BUFFER_SIZE, BUFFER_SIZE);
    printf("==================================================================\n\n");

    printf("Select Synchronization Mode:\n");
    printf("  1. Shared File Synchronization (Producer: File Write | Consumer: File Read)\n");
    printf("  2. Synchronized File Transfer   (Producer: File Read  | Consumer: File Write)\n");
    printf("  3. Interactive Character Stream (Classic In-Memory Bounded Buffer)\n");
    printf("Enter choice (1-3) [Default: 1]: ");

    char mode_input[16];
    int choice = 1;
    if (fgets(mode_input, sizeof(mode_input), stdin) != NULL) {
        if (mode_input[0] >= '1' && mode_input[0] <= '3') {
            choice = mode_input[0] - '0';
        }
    }

    // Broadcast mode to Consumer via shared memory
    buffer->mode = choice;

    // -------------------------------------------------------------------------
    // MODE 1: Shared File Synchronization
    // Producer writes data records into a shared file (File Write).
    // Consumer reads records from the shared file (File Read).
    // Semaphores ensure mutual exclusion and prevent buffer overflow/underflow.
    // -------------------------------------------------------------------------
    if (choice == MODE_SHARED_FILE) {
        printf("\n--- Mode 1: Shared File Synchronization ---\n");
        printf("Enter shared file name [Default: %s]: ", DEFAULT_SHARED_FILE);

        char fname[256];
        if (fgets(fname, sizeof(fname), stdin) != NULL) {
            fname[strcspn(fname, "\r\n")] = '\0';
        }
        if (strlen(fname) == 0) {
            strcpy(buffer->shared_filename, DEFAULT_SHARED_FILE);
        } else {
            strncpy(buffer->shared_filename, fname, sizeof(buffer->shared_filename) - 1);
        }

        // Initialize / truncate shared file
        FILE *init_fp = fopen(buffer->shared_filename, "w");
        if (init_fp) fclose(init_fp);

        printf("[Producer] Shared file ready: '%s'\n", buffer->shared_filename);
        printf("\nChoose Data Input Option:\n");
        printf("  1. Enter text records interactively (type 'EXIT' to finish)\n");
        printf("  2. Automatically write 6 sample records (demonstrates blocking)\n");
        printf("Enter option (1-2) [Default: 1]: ");

        int opt = 1;
        char opt_input[16];
        if (fgets(opt_input, sizeof(opt_input), stdin) != NULL) {
            if (opt_input[0] == '2') opt = 2;
        }

        if (opt == 1) {
            printf("\nType lines to produce and write to '%s'. Type 'EXIT' to finish:\n", buffer->shared_filename);
            char line[512];
            int item_no = 0;

            while (1) {
                printf("[Producer - Input #%d]: ", item_no + 1);
                if (fgets(line, sizeof(line), stdin) == NULL) break;
                line[strcspn(line, "\r\n")] = '\0';

                if (strcmp(line, "EXIT") == 0 || strcmp(line, "exit") == 0) {
                    break;
                }

                // Classical Synchronization:
                sem_wait_op(semid, SEM_EMPTY); // Wait for an empty slot (blocks if 5 items unread)
                sem_wait_op(semid, SEM_MUTEX); // Acquire exclusive file access lock

                // --- CRITICAL SECTION: FILE WRITE ---
                FILE *fp = fopen(buffer->shared_filename, "a");
                if (fp) {
                    fprintf(fp, "%s\n", line);
                    fflush(fp);
                    fclose(fp);
                }
                item_no++;
                buffer->total_items = item_no;
                int slot = buffer->in;
                buffer->in = (buffer->in + 1) % BUFFER_SIZE;

                printf(" -> [Produced - File Write] Wrote item #%d to '%s' (Slot [%d]): \"%s\"\n", 
                       item_no, buffer->shared_filename, slot, line);
                printf("    [Semaphores] SEM_EMPTY: %d | SEM_FULL: %d\n",
                       sem_get_val(semid, SEM_EMPTY), sem_get_val(semid, SEM_FULL) + 1);

                sem_signal_op(semid, SEM_MUTEX); // Release file lock
                sem_signal_op(semid, SEM_FULL);  // Signal Consumer that an item is available
            }
        } else {
            // Auto-generate records
            printf("\n[Producer] Auto-generating 6 records into '%s'...\n", buffer->shared_filename);
            for (int i = 1; i <= 6; i++) {
                char record[128];
                snprintf(record, sizeof(record), "Telemetry Record #%d: Status OK, Timestamp=%ld", i, (long)(time(NULL) + i));

                printf("[Producer] Waiting to write Record #%d (will block if buffer is full)...\n", i);
                sem_wait_op(semid, SEM_EMPTY);
                sem_wait_op(semid, SEM_MUTEX);

                // --- CRITICAL SECTION: FILE WRITE ---
                FILE *fp = fopen(buffer->shared_filename, "a");
                if (fp) {
                    fprintf(fp, "%s\n", record);
                    fflush(fp);
                    fclose(fp);
                }
                buffer->total_items = i;
                int slot = buffer->in;
                buffer->in = (buffer->in + 1) % BUFFER_SIZE;

                printf(" -> [Produced - File Write] Wrote Record #%d (Slot [%d])\n", i, slot);

                sem_signal_op(semid, SEM_MUTEX);
                sem_signal_op(semid, SEM_FULL);

                usleep(400000); // 400ms delay between productions
            }
        }

        // Write EOF Sentinel to shared file under synchronization
        sem_wait_op(semid, SEM_EMPTY);
        sem_wait_op(semid, SEM_MUTEX);

        FILE *fp = fopen(buffer->shared_filename, "a");
        if (fp) {
            fprintf(fp, "__EOF__\n");
            fflush(fp);
            fclose(fp);
        }
        buffer->is_finished = 1;
        printf("\n[Producer] Produced EOF marker '__EOF__' into '%s'.\n", buffer->shared_filename);

        sem_signal_op(semid, SEM_MUTEX);
        sem_signal_op(semid, SEM_FULL);
    }
    // -------------------------------------------------------------------------
    // MODE 2: Synchronized File Transfer (Read Source File -> Write Destination)
    // Producer reads file chunks from source file (File Read).
    // Places chunks into bounded shared memory buffer.
    // Consumer reads from buffer and writes to destination file (File Write).
    // -------------------------------------------------------------------------
    else if (choice == MODE_FILE_TRANSFER) {
        printf("\n--- Mode 2: Synchronized File Transfer via Bounded Buffer ---\n");
        printf("Enter source file to read [Default: sample_input.txt]: ");

        char src[256];
        if (fgets(src, sizeof(src), stdin) != NULL) {
            src[strcspn(src, "\r\n")] = '\0';
        }
        if (strlen(src) == 0) {
            strcpy(buffer->source_filename, "sample_input.txt");
        } else {
            strncpy(buffer->source_filename, src, sizeof(buffer->source_filename) - 1);
        }

        // Create sample file if source doesn't exist
        FILE *check = fopen(buffer->source_filename, "rb");
        if (!check) {
            printf("[Producer] Source file '%s' not found. Creating sample file...\n", buffer->source_filename);
            create_sample_file(buffer->source_filename);
        } else {
            fclose(check);
        }

        printf("Enter destination file for Consumer [Default: output.txt]: ");
        char dst[256];
        if (fgets(dst, sizeof(dst), stdin) != NULL) {
            dst[strcspn(dst, "\r\n")] = '\0';
        }
        if (strlen(dst) == 0) {
            strcpy(buffer->dest_filename, "output.txt");
        } else {
            strncpy(buffer->dest_filename, dst, sizeof(buffer->dest_filename) - 1);
        }

        FILE *fp_src = fopen(buffer->source_filename, "rb");
        if (!fp_src) {
            perror("Failed to open source file");
            shmdt(buffer);
            exit(1);
        }

        printf("\n[Producer] Reading '%s' and streaming chunks into bounded buffer...\n", buffer->source_filename);

        char temp_chunk[CHUNK_SIZE];
        int bytes_read;
        int chunk_num = 0;

        while (1) {
            bytes_read = fread(temp_chunk, 1, CHUNK_SIZE, fp_src);
            int is_eof = feof(fp_src) ? 1 : 0;

            sem_wait_op(semid, SEM_EMPTY); // Wait for an empty slot in bounded buffer
            sem_wait_op(semid, SEM_MUTEX); // Enter Critical Section

            // --- CRITICAL SECTION: WRITE TO BOUNDED BUFFER ---
            int slot = buffer->in;
            buffer->chunk_data[slot].size = bytes_read;
            buffer->chunk_data[slot].is_eof = is_eof;
            if (bytes_read > 0) {
                memcpy(buffer->chunk_data[slot].data, temp_chunk, bytes_read);
            }
            buffer->in = (buffer->in + 1) % BUFFER_SIZE;
            chunk_num++;
            buffer->total_items = chunk_num;

            printf(" -> [Produced - File Read] Read %d bytes from '%s' -> Buffer slot [%d] (Chunk #%d, EOF=%d)\n",
                   bytes_read, buffer->source_filename, slot, chunk_num, is_eof);

            sem_signal_op(semid, SEM_MUTEX); // Exit Critical Section
            sem_signal_op(semid, SEM_FULL);  // Signal Consumer that chunk is ready

            if (is_eof) break;
            usleep(200000);
        }

        fclose(fp_src);
        buffer->is_finished = 1;
        printf("[Producer] Source file '%s' reading complete. %d chunks produced.\n", buffer->source_filename, chunk_num);
    }
    // -------------------------------------------------------------------------
    // MODE 3: Classic Interactive Character Stream
    // -------------------------------------------------------------------------
    else {
        printf("\n--- Mode 3: Classic Interactive Character Stream ---\n");
        char ch;
        while (1) {
            printf("[Producer] Enter character to produce ('q' to quit): ");
            if (scanf(" %c", &ch) != 1) break;

            if (ch == 'q') {
                sem_wait_op(semid, SEM_EMPTY);
                sem_wait_op(semid, SEM_MUTEX);

                buffer->char_data[buffer->in] = 'q';
                buffer->in = (buffer->in + 1) % BUFFER_SIZE;
                buffer->is_finished = 1;

                sem_signal_op(semid, SEM_MUTEX);
                sem_signal_op(semid, SEM_FULL);

                printf("[Producer] Produced quit signal 'q'. Exiting loop.\n");
                break;
            }

            sem_wait_op(semid, SEM_EMPTY);
            sem_wait_op(semid, SEM_MUTEX);

            buffer->char_data[buffer->in] = ch;
            int slot = buffer->in;
            buffer->in = (buffer->in + 1) % BUFFER_SIZE;
            printf(" -> [Produced] '%c' inserted at buffer slot [%d]\n", ch, slot);

            sem_signal_op(semid, SEM_MUTEX);
            sem_signal_op(semid, SEM_FULL);
        }
    }

    // Detach from shared memory
    shmdt(buffer);
    printf("\n[Producer] Detached from shared memory. Done.\n");

    return 0;
}

#include <stdio.h>

// Helper function to print frame contents
void printStep(int page, int frames[], int n, const char *tag) {
    printf("Page %2d -> [ ", page);
    for (int i = 0; i < n; i++) {
        if (frames[i] != -1) printf("%2d ", frames[i]);
        else printf(" - ");
    }
    printf("] %s\n", tag);
}

// ---------------- 1. FIFO (First-In, First-Out) ----------------
int FIFO(int noOfFrames, int noOfPageRef, int *pageLst) {
    int frames[noOfFrames], faults = 0, fifo_idx = 0;
    for (int i = 0; i < noOfFrames; i++) frames[i] = -1;

    printf("\n--- FIFO Page Replacement ---\n");
    for (int i = 0; i < noOfPageRef; i++) {
        int page = pageLst[i], hit = 0;

        // Check for page hit
        for (int j = 0; j < noOfFrames; j++) {
            if (frames[j] == page) { hit = 1; break; }
        }

        if (hit) {
            printStep(page, frames, noOfFrames, "[HIT]");
        } else {
            frames[fifo_idx] = page;
            fifo_idx = (fifo_idx + 1) % noOfFrames; // Circular queue replacement
            faults++;
            printStep(page, frames, noOfFrames, "[FAULT]");
        }
    }
    return faults;
}

// ---------------- 2. LRU (Least Recently Used) ----------------
int LRU(int noOfFrames, int noOfPageRef, int *pageLst) {
    int frames[noOfFrames], faults = 0;
    for (int i = 0; i < noOfFrames; i++) frames[i] = -1;

    printf("\n--- LRU Page Replacement ---\n");
    for (int i = 0; i < noOfPageRef; i++) {
        int page = pageLst[i], hit = 0;

        for (int j = 0; j < noOfFrames; j++) {
            if (frames[j] == page) { hit = 1; break; }
        }

        if (hit) {
            printStep(page, frames, noOfFrames, "[HIT]");
        } else {
            faults++;
            // Check for any empty frame first
            int replace_idx = -1;
            for (int j = 0; j < noOfFrames; j++) {
                if (frames[j] == -1) { replace_idx = j; break; }
            }

            // If full, find the page least recently used in the past
            if (replace_idx == -1) {
                int least_recent = i;
                for (int j = 0; j < noOfFrames; j++) {
                    for (int k = i - 1; k >= 0; k--) {
                        if (pageLst[k] == frames[j]) {
                            if (k < least_recent) {
                                least_recent = k;
                                replace_idx = j;
                            }
                            break;
                        }
                    }
                }
            }

            frames[replace_idx] = page;
            printStep(page, frames, noOfFrames, "[FAULT]");
        }
    }
    return faults;
}

// ---------------- 3. OPTIMAL (Belady's Optimal) ----------------
int OPTIMAL(int noOfFrames, int noOfPageRef, int *pageLst) {
    int frames[noOfFrames], faults = 0;
    for (int i = 0; i < noOfFrames; i++) frames[i] = -1;

    printf("\n--- OPTIMAL Page Replacement ---\n");
    for (int i = 0; i < noOfPageRef; i++) {
        int page = pageLst[i], hit = 0;

        for (int j = 0; j < noOfFrames; j++) {
            if (frames[j] == page) { hit = 1; break; }
        }

        if (hit) {
            printStep(page, frames, noOfFrames, "[HIT]");
        } else {
            faults++;
            int replace_idx = -1;
            for (int j = 0; j < noOfFrames; j++) {
                if (frames[j] == -1) { replace_idx = j; break; }
            }

            // If full, find page not used for the longest time in the future
            if (replace_idx == -1) {
                int farthest = -1;
                for (int j = 0; j < noOfFrames; j++) {
                    int next_use = noOfPageRef; // Assume infinity if not used again
                    for (int k = i + 1; k < noOfPageRef; k++) {
                        if (pageLst[k] == frames[j]) {
                            next_use = k;
                            break;
                        }
                    }
                    if (next_use > farthest) {
                        farthest = next_use;
                        replace_idx = j;
                    }
                }
            }

            frames[replace_idx] = page;
            printStep(page, frames, noOfFrames, "[FAULT]");
        }
    }
    return faults;
}

int main() {
    int noOfFrames, noOfPageRef;

    printf("Enter number of frames: ");
    scanf("%d", &noOfFrames);

    printf("Enter number of page references: ");
    scanf("%d", &noOfPageRef);

    int pageLst[noOfPageRef];
    printf("Enter the page reference string: ");
    for (int i = 0; i < noOfPageRef; i++) {
        scanf("%d", &pageLst[i]);
    }

    // Execute algorithms
    int fifo_faults = FIFO(noOfFrames, noOfPageRef, pageLst);
    int lru_faults  = LRU(noOfFrames, noOfPageRef, pageLst);
    int opt_faults  = OPTIMAL(noOfFrames, noOfPageRef, pageLst);

    // Final Comparison Summary
    printf("\n======== SUMMARY ========\n");
    printf("FIFO    page faults : %d\n", fifo_faults);
    printf("LRU     page faults : %d\n", lru_faults);
    printf("OPTIMAL page faults : %d\n", opt_faults);
    printf("=========================\n");

    return 0;
}

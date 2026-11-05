#include <stdio.h>
#include <stdlib.h>

#define MAX 50

// Function to display the allocation results in a clean table
void displayResults(int process_size[], int n, int allocated_hole[], int original_hole_size[], const char *algo_name) {
    printf("\n=========================================================================\n");
    printf("                  Strategy: %s\n", algo_name);
    printf("=========================================================================\n");
    printf("| Process | Process Size | Allocated Hole | Hole Size | Internal Fragment |\n");
    printf("-------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        if (allocated_hole[i] != -1) {
            int h = allocated_hole[i];
            int fragment = original_hole_size[h] - process_size[i];
            printf("|   P%-4d |     %4d     |       H%-7d|   %5d   |       %4d        |\n",
                   i + 1, process_size[i], h + 1, original_hole_size[h], fragment);
        } else {
            printf("|   P%-4d |     %4d     |  Not Allocated |     -     |         -         |\n",
                   i + 1, process_size[i]);
        }
    }
    printf("=========================================================================\n\n");
}

// ---------------- 1. First Fit ----------------
// Allocates the first available hole that is large enough
void firstFit(int process_size[], int n, int hole_size[], int m) {
    int allocated_hole[MAX];
    int hole_used[MAX] = {0}; // 0 = free, 1 = occupied

    for (int i = 0; i < n; i++) {
        allocated_hole[i] = -1; // Initially not allocated

        for (int j = 0; j < m; j++) {
            if (!hole_used[j] && hole_size[j] >= process_size[i]) {
                allocated_hole[i] = j;
                hole_used[j] = 1; // Mark hole as used
                break; // Take the first matching hole
            }
        }
    }

    displayResults(process_size, n, allocated_hole, hole_size, "First Fit");
}

// ---------------- 2. Best Fit ----------------
// Allocates the smallest available hole that fits the process (minimizes leftover space)
void bestFit(int process_size[], int n, int hole_size[], int m) {
    int allocated_hole[MAX];
    int hole_used[MAX] = {0};

    for (int i = 0; i < n; i++) {
        allocated_hole[i] = -1;
        int best_idx = -1;

        for (int j = 0; j < m; j++) {
            if (!hole_used[j] && hole_size[j] >= process_size[i]) {
                if (best_idx == -1 || hole_size[j] < hole_size[best_idx]) {
                    best_idx = j;
                }
            }
        }

        if (best_idx != -1) {
            allocated_hole[i] = best_idx;
            hole_used[best_idx] = 1; // Mark hole as used
        }
    }

    displayResults(process_size, n, allocated_hole, hole_size, "Best Fit");
}

// ---------------- 3. Worst Fit ----------------
// Allocates the largest available hole that fits the process (maximizes leftover space)
void worstFit(int process_size[], int n, int hole_size[], int m) {
    int allocated_hole[MAX];
    int hole_used[MAX] = {0};

    for (int i = 0; i < n; i++) {
        allocated_hole[i] = -1;
        int worst_idx = -1;

        for (int j = 0; j < m; j++) {
            if (!hole_used[j] && hole_size[j] >= process_size[i]) {
                if (worst_idx == -1 || hole_size[j] > hole_size[worst_idx]) {
                    worst_idx = j;
                }
            }
        }

        if (worst_idx != -1) {
            allocated_hole[i] = worst_idx;
            hole_used[worst_idx] = 1; // Mark hole as used
        }
    }

    displayResults(process_size, n, allocated_hole, hole_size, "Worst Fit");
}

int main() {
    int n, m;
    int process_size[MAX], hole_size[MAX];
    int choice;

    printf("====================================================\n");
    printf("    Contiguous Memory Allocation Schemes (EXP6)     \n");
    printf("====================================================\n");

    // Input Holes / Memory Blocks
    printf("Enter total number of memory holes (m): ");
    scanf("%d", &m);
    printf("Enter sizes of %d memory holes (H1 to H%d):\n", m, m);
    for (int j = 0; j < m; j++) {
        printf("  Hole H%d size: ", j + 1);
        scanf("%d", &hole_size[j]);
    }

    // Input Processes
    printf("\nEnter total number of processes (n): ");
    scanf("%d", &n);
    printf("Enter memory requirements of %d processes (P1 to P%d):\n", n, n);
    for (int i = 0; i < n; i++) {
        printf("  Process P%d size: ", i + 1);
        scanf("%d", &process_size[i]);
    }

    do {
        printf("\n---------------- MEMORY ALLOCATION MENU ----------------\n");
        printf("1. First Fit Allocation\n");
        printf("2. Best Fit Allocation\n");
        printf("3. Worst Fit Allocation\n");
        printf("4. Re-enter Holes and Processes Dataset\n");
        printf("5. Exit\n");
        printf("--------------------------------------------------------\n");
        printf("Enter your choice (1-5): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                firstFit(process_size, n, hole_size, m);
                break;

            case 2:
                bestFit(process_size, n, hole_size, m);
                break;

            case 3:
                worstFit(process_size, n, hole_size, m);
                break;

            case 4:
                printf("\nEnter total number of memory holes (m): ");
                scanf("%d", &m);
                printf("Enter sizes of %d memory holes:\n", m);
                for (int j = 0; j < m; j++) {
                    printf("  Hole H%d size: ", j + 1);
                    scanf("%d", &hole_size[j]);
                }

                printf("\nEnter total number of processes (n): ");
                scanf("%d", &n);
                printf("Enter memory requirements of %d processes:\n", n);
                for (int i = 0; i < n; i++) {
                    printf("  Process P%d size: ", i + 1);
                    scanf("%d", &process_size[i]);
                }
                break;

            case 5:
                printf("Exiting Memory Allocation Simulator. Good luck!\n");
                break;

            default:
                printf("Invalid option! Please enter between 1 and 5.\n");
        }
    } while (choice != 5);

    return 0;
}

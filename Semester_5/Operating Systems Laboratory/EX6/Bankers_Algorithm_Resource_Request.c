#include <stdio.h>

int n, m, alloc[10][10], max[10][10], need[10][10], avail[10];

// Safety Algorithm
int isSafe(int seq[]) {
    int work[10], finish[10] = {0}, count = 0;
    for (int j = 0; j < m; j++) work[j] = avail[j];

    while (count < n) {
        int found = 0;
        for (int i = 0; i < n; i++) {
            if (!finish[i]) {
                int can_allocate = 1;
                for (int j = 0; j < m; j++)
                    if (need[i][j] > work[j]) { can_allocate = 0; break; }

                if (can_allocate) {
                    for (int j = 0; j < m; j++) work[j] += alloc[i][j];
                    seq[count++] = i;
                    finish[i] = 1;
                    found = 1;
                }
            }
        }
        if (!found) return 0; // Unsafe
    }
    return 1; // Safe
}

// Resource-Request Algorithm
void requestResource(int p, int req[]) {
    for (int j = 0; j < m; j++) {
        if (req[j] > need[p][j]) { printf("Error: Exceeded max claim!\n"); return; }
        if (req[j] > avail[j])    { printf("P%d must wait! (Resources unavailable)\n", p); return; }
    }

    // Pretend allocation
    for (int j = 0; j < m; j++) {
        avail[j] -= req[j]; alloc[p][j] += req[j]; need[p][j] -= req[j];
    }

    int seq[10];
    if (isSafe(seq)) {
        printf("Request GRANTED! Safe Sequence: ");
        for (int i = 0; i < n; i++) printf("P%d ", seq[i]);
        printf("\n");
    } else {
        printf("Request DENIED (Unsafe)! Rolling back.\n");
        for (int j = 0; j < m; j++) { // Rollback
            avail[j] += req[j]; alloc[p][j] -= req[j]; need[p][j] += req[j];
        }
    }
}

int main() {
    printf("Enter number of processes & resources: ");
    scanf("%d %d", &n, &m);

    printf("Enter Allocation Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) scanf("%d", &alloc[i][j]);

    printf("Enter Max Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) scanf("%d", &max[i][j]);

    printf("Enter Available Vector: ");
    for (int j = 0; j < m; j++) scanf("%d", &avail[j]);

    // Calculate & display Need Matrix
    printf("\nNeed Matrix:\n");
    for (int i = 0; i < n; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < m; j++) {
            need[i][j] = max[i][j] - alloc[i][j];
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }

    // Initial Safety Check
    int seq[10];
    if (isSafe(seq)) {
        printf("\nSystem is in SAFE state.\nSafe Sequence: ");
        for (int i = 0; i < n; i++) printf("P%d ", seq[i]);
        printf("\n");

        // Handle Resource Request
        int p, req[10];
        printf("\nEnter Process ID making request: ");
        scanf("%d", &p);
        printf("Enter request vector: ");
        for (int j = 0; j < m; j++) scanf("%d", &req[j]);

        requestResource(p, req);
    } else {
        printf("\nSystem is in UNSAFE state!\n");
    }

    return 0;
}

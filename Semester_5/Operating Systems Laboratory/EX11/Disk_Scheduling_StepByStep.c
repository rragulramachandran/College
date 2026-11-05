#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Print table header and steps
void printHeader(const char *name) {
    printf("\n=======================================================================\n");
    printf("                  Algorithm: %s\n", name);
    printf("=======================================================================\n");
    printf("| Current Pos  | Destination  | Head Movement | Cumulative Move | Action         |\n");
    printf("-----------------------------------------------------------------------\n");
}

void printStep(int from, int to, int *cum, const char *action) {
    int diff = abs(to - from);
    *cum += diff;
    printf("|     %-8d |     %-8d |      %-8d |      %-10d | %-14s |\n", from, to, diff, *cum, action);
}

// Simple ascending bubble sort
void sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
            }
        }
    }
}

// ---------------- 1. FCFS ----------------
int FCFS(int *cylst, int noOfReq, int totalCyl, int initialHeadPos) {
    printHeader("FCFS (First-Come, First-Served)");
    int cur = initialHeadPos, total = 0;
    for (int i = 0; i < noOfReq; i++) {
        printStep(cur, cylst[i], &total, "Request");
        cur = cylst[i];
    }
    printf("=======================================================================\n");
    printf("Total Head Movement (FCFS) = %d\n", total);
    return total;
}

// ---------------- 2. SSTF ----------------
int SSTF(int *cylst, int noOfReq, int totalCyl, int initialHeadPos) {
    printHeader("SSTF (Shortest Seek Time First)");
    int visited[noOfReq], cur = initialHeadPos, total = 0;
    for (int i = 0; i < noOfReq; i++) visited[i] = 0;

    for (int count = 0; count < noOfReq; count++) {
        int min_dist = 999999, idx = -1;
        for (int i = 0; i < noOfReq; i++) {
            if (!visited[i]) {
                int d = abs(cylst[i] - cur);
                if (d < min_dist) { min_dist = d; idx = i; }
            }
        }
        visited[idx] = 1;
        printStep(cur, cylst[idx], &total, "Request");
        cur = cylst[idx];
    }
    printf("=======================================================================\n");
    printf("Total Head Movement (SSTF) = %d\n", total);
    return total;
}

// ---------------- 3. SCAN ----------------
int SCAN(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction) {
    printHeader("SCAN (Elevator)");
    int left[noOfReq], right[noOfReq], l_cnt = 0, r_cnt = 0, total = 0, cur = initialHeadPos;

    for (int i = 0; i < noOfReq; i++) {
        if (cylst[i] < initialHeadPos) left[l_cnt++] = cylst[i];
        else right[r_cnt++] = cylst[i];
    }
    sort(left, l_cnt);
    sort(right, r_cnt);

    if (direction == 1) { // High
        for (int i = 0; i < r_cnt; i++) { printStep(cur, right[i], &total, "Request"); cur = right[i]; }
        if (l_cnt > 0) {
            printStep(cur, totalCyl - 1, &total, "Disk end"); cur = totalCyl - 1;
            for (int i = l_cnt - 1; i >= 0; i--) { printStep(cur, left[i], &total, "Request"); cur = left[i]; }
        }
    } else { // Low
        for (int i = l_cnt - 1; i >= 0; i--) { printStep(cur, left[i], &total, "Request"); cur = left[i]; }
        if (r_cnt > 0) {
            printStep(cur, 0, &total, "Disk end"); cur = 0;
            for (int i = 0; i < r_cnt; i++) { printStep(cur, right[i], &total, "Request"); cur = right[i]; }
        }
    }
    printf("=======================================================================\n");
    printf("Total Head Movement (SCAN) = %d\n", total);
    return total;
}

// ---------------- 4. C-SCAN ----------------
int CSCAN(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction) {
    printHeader("C-SCAN (Circular SCAN)");
    int left[noOfReq], right[noOfReq], l_cnt = 0, r_cnt = 0, total = 0, cur = initialHeadPos;

    for (int i = 0; i < noOfReq; i++) {
        if (cylst[i] < initialHeadPos) left[l_cnt++] = cylst[i];
        else right[r_cnt++] = cylst[i];
    }
    sort(left, l_cnt);
    sort(right, r_cnt);

    if (direction == 1) { // High
        for (int i = 0; i < r_cnt; i++) { printStep(cur, right[i], &total, "Request"); cur = right[i]; }
        if (l_cnt > 0) {
            printStep(cur, totalCyl - 1, &total, "Disk end"); cur = totalCyl - 1;
            printStep(cur, 0, &total, "Circular jump"); cur = 0;
            for (int i = 0; i < l_cnt; i++) { printStep(cur, left[i], &total, "Request"); cur = left[i]; }
        }
    } else { // Low
        for (int i = l_cnt - 1; i >= 0; i--) { printStep(cur, left[i], &total, "Request"); cur = left[i]; }
        if (r_cnt > 0) {
            printStep(cur, 0, &total, "Disk end"); cur = 0;
            printStep(cur, totalCyl - 1, &total, "Circular jump"); cur = totalCyl - 1;
            for (int i = r_cnt - 1; i >= 0; i--) { printStep(cur, right[i], &total, "Request"); cur = right[i]; }
        }
    }
    printf("=======================================================================\n");
    printf("Total Head Movement (C-SCAN) = %d\n", total);
    return total;
}

// ---------------- 5. LOOK ----------------
int LOOK(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction) {
    printHeader("LOOK");
    int left[noOfReq], right[noOfReq], l_cnt = 0, r_cnt = 0, total = 0, cur = initialHeadPos;

    for (int i = 0; i < noOfReq; i++) {
        if (cylst[i] < initialHeadPos) left[l_cnt++] = cylst[i];
        else right[r_cnt++] = cylst[i];
    }
    sort(left, l_cnt);
    sort(right, r_cnt);

    if (direction == 1) { // High
        for (int i = 0; i < r_cnt; i++) { printStep(cur, right[i], &total, "Request"); cur = right[i]; }
        for (int i = l_cnt - 1; i >= 0; i--) { printStep(cur, left[i], &total, "Request"); cur = left[i]; }
    } else { // Low
        for (int i = l_cnt - 1; i >= 0; i--) { printStep(cur, left[i], &total, "Request"); cur = left[i]; }
        for (int i = 0; i < r_cnt; i++) { printStep(cur, right[i], &total, "Request"); cur = right[i]; }
    }
    printf("=======================================================================\n");
    printf("Total Head Movement (LOOK) = %d\n", total);
    return total;
}

// ---------------- 6. C-LOOK ----------------
int CLOOK(int *cylst, int noOfReq, int totalCyl, int initialHeadPos, int direction) {
    printHeader("C-LOOK (Circular LOOK)");
    int left[noOfReq], right[noOfReq], l_cnt = 0, r_cnt = 0, total = 0, cur = initialHeadPos;

    for (int i = 0; i < noOfReq; i++) {
        if (cylst[i] < initialHeadPos) left[l_cnt++] = cylst[i];
        else right[r_cnt++] = cylst[i];
    }
    sort(left, l_cnt);
    sort(right, r_cnt);

    if (direction == 1) { // High
        for (int i = 0; i < r_cnt; i++) { printStep(cur, right[i], &total, "Request"); cur = right[i]; }
        if (l_cnt > 0) {
            printStep(cur, left[0], &total, "Circular jump"); cur = left[0];
            for (int i = 1; i < l_cnt; i++) { printStep(cur, left[i], &total, "Request"); cur = left[i]; }
        }
    } else { // Low
        for (int i = l_cnt - 1; i >= 0; i--) { printStep(cur, left[i], &total, "Request"); cur = left[i]; }
        if (r_cnt > 0) {
            printStep(cur, right[r_cnt - 1], &total, "Circular jump"); cur = right[r_cnt - 1];
            for (int i = r_cnt - 2; i >= 0; i--) { printStep(cur, right[i], &total, "Request"); cur = right[i]; }
        }
    }
    printf("=======================================================================\n");
    printf("Total Head Movement (C-LOOK) = %d\n", total);
    return total;
}

int main() {
    int noOfReq, totalCyl, initialHeadPos, direction;

    printf("Enter total number of disk requests (N): ");
    scanf("%d", &noOfReq);

    int cylst[noOfReq];
    printf("Enter sequence of cylinder requests: ");
    for (int i = 0; i < noOfReq; i++) scanf("%d", &cylst[i]);

    printf("Enter total number of cylinders (C): ");
    scanf("%d", &totalCyl);

    printf("Enter initial head position: ");
    scanf("%d", &initialHeadPos);

    printf("Enter direction (1 for High / Right, 0 for Low / Left): ");
    scanf("%d", &direction);

    // Run all 6 algorithms
    int fcfs  = FCFS(cylst, noOfReq, totalCyl, initialHeadPos);
    int sstf  = SSTF(cylst, noOfReq, totalCyl, initialHeadPos);
    int scan  = SCAN(cylst, noOfReq, totalCyl, initialHeadPos, direction);
    int cscan = CSCAN(cylst, noOfReq, totalCyl, initialHeadPos, direction);
    int look  = LOOK(cylst, noOfReq, totalCyl, initialHeadPos, direction);
    int clook = CLOOK(cylst, noOfReq, totalCyl, initialHeadPos, direction);

    // Summary Comparison Table
    printf("\n=======================================================================\n");
    printf("                       FINAL COMPARISON SUMMARY                        \n");
    printf("=======================================================================\n");
    printf("| Algorithm | Total Head Movement | Efficiency Ranking                |\n");
    printf("-----------------------------------------------------------------------\n");
    printf("| SSTF      |         %-11d | Best (tied)                       |\n", sstf);
    printf("| LOOK      |         %-11d | Best (tied)                       |\n", look);
    printf("| C-LOOK    |         %-11d | Good                              |\n", clook);
    printf("| SCAN      |         %-11d | Moderate                          |\n", scan);
    printf("| C-SCAN    |         %-11d | Higher overhead                   |\n", cscan);
    printf("| FCFS      |         %-11d | Worst                             |\n", fcfs);
    printf("=======================================================================\n");

    return 0;
}

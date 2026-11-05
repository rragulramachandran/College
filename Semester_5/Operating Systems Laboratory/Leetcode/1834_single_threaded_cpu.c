#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int start, duration, index;

} Task;

int compare(const void *a, const void *b) {
    const Task *x = a, *y = b;

    if (x->start != y->start)
        return (x->start > y->start) - (x->start < y->start);

    return (x->index > y->index) - (x->index < y->index);
}

int less(Task a, Task b) {
    if (a.duration != b.duration)
        return a.duration < b.duration;

    return a.index < b.index;
}

void push(Task heap[], int *size, Task t) {
    int i = (*size)++;

    while (i > 0) {
        int p = (i - 1) / 2;

        if (!less(t, heap[p]))
            break;

        heap[i] = heap[p];
        i = p;
    }

    heap[i] = t;
}

Task pop(Task heap[], int *size) {
    Task result = heap[0];
    Task last = heap[--(*size)];

    if (*size == 0)
        return result;

    int i = 0;

    while (2 * i + 1 < *size) {
        int child = 2 * i + 1;

        if (child + 1 < *size &&

            less(heap[child + 1], heap[child]))
            child++;

        if (!less(heap[child], last))
            break;

        heap[i] = heap[child];
        i = child;
    }

    heap[i] = last;
    return result;
}

int *getOrder(int **tasks, int tasksSize, int *tasksColSize, int *returnSize) {

    int n = tasksSize;

    Task *arr = malloc(n * sizeof(Task));
    Task *heap = malloc(n * sizeof(Task));
    int *answer = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        arr[i] = (Task){tasks[i][0], tasks[i][1], i};
    }

    qsort(arr, n, sizeof(Task), compare);

    long long time = 0;
    int i = 0, size = 0, count = 0;

    while (count < n) {

        if (size == 0 && time < arr[i].start)

            time = arr[i].start;

        while (i < n && arr[i].start <= time)
            push(heap, &size, arr[i++]);

        Task current = pop(heap, &size);

        answer[count++] = current.index;
        time += current.duration;
    }

    free(arr);
    free(heap);

    *returnSize = n;
    return answer;
}

#include <stdlib.h>
typedef struct {
    int weight;
    int index;

    long long time;
} Server;

int availableLess(Server a, Server b) {
    if (a.weight != b.weight)
        return a.weight < b.weight;
    return a.index < b.index;
}

int busyLess(Server a, Server b) {

    if (a.time != b.time)
        return a.time < b.time;

    if (a.weight != b.weight)
        return a.weight < b.weight;
    return a.index < b.index;
}

void push(Server heap[], int *size, Server s, int (*less)(Server, Server)) {

    int i = (*size)++;

    while (i > 0) {
        int p = (i - 1) / 2;

        if (!less(s, heap[p]))
            break;

        heap[i] = heap[p];
        i = p;
    }

    heap[i] = s;
}

Server pop(Server heap[], int *size, int (*less)(Server, Server)) {

    Server result = heap[0];
    Server last = heap[--(*size)];

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

int *assignTasks(int *servers, int serversSize, int *tasks, int tasksSize,
                 int *returnSize) {

    int n = serversSize;
    int m = tasksSize;

    Server *available = malloc(n * sizeof(Server));
    Server *busy = malloc(n * sizeof(Server));
    int *ans = malloc(m * sizeof(int));

    int freeSize = 0, busySize = 0;
    long long time = 0;

    for (int i = 0; i < n; i++) {
        Server s = {servers[i], i, 0};
        push(available, &freeSize, s, availableLess);
    }

    for (int i = 0; i < m; i++) {

        if (time < i)
            time = i;

        if (freeSize == 0 && time < busy[0].time)
            time = busy[0].time;

        while (busySize > 0 && busy[0].time <= time) {
            Server s = pop(busy, &busySize, busyLess);
            push(available, &freeSize, s, availableLess);
        }

        Server s = pop(available, &freeSize, availableLess);

        ans[i] = s.index;
        s.time = time + tasks[i];

        push(busy, &busySize, s, busyLess);
    }

    free(available);

    free(busy);

    *returnSize = m;
    return ans;
}

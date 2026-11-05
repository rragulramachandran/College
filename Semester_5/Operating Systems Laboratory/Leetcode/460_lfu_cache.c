#include <stdlib.h>

#define MAX_KEY 100001
#define MAX_FREQ 200005

typedef struct Node {
    int key, value, freq;
    struct Node *prev, *next;
} Node;

typedef struct {
    Node *head, *tail;
} List;

typedef struct {
    int capacity, size, minFreq;
    Node *map[MAX_KEY];
    List *freqList[MAX_FREQ];
} LFUCache;

Node *createNode(int key, int value) {
    Node *node = malloc(sizeof(Node));

    node->key = key;
    node->value = value;
    node->freq = 1;
    node->prev = node->next = NULL;

    return node;
}

List *createList() {
    List *list = malloc(sizeof(List));

    list->head = calloc(1, sizeof(Node));
    list->tail = calloc(1, sizeof(Node));

    list->head->next = list->tail;
    list->tail->prev = list->head;

    return list;
}

void removeNode(Node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void addFront(List *list, Node *node) {
    node->next = list->head->next;
    node->prev = list->head;

    list->head->next->prev = node;
    list->head->next = node;
}

void updateFreq(LFUCache *obj, Node *node) {
    int freq = node->freq;
    List *oldList = obj->freqList[freq];

    removeNode(node);

    if (freq == obj->minFreq && oldList->head->next == oldList->tail) {
        obj->minFreq++;
    }

    node->freq++;

    if (obj->freqList[node->freq] == NULL) {
        obj->freqList[node->freq] = createList();
    }

    addFront(obj->freqList[node->freq], node);
}

LFUCache *lFUCacheCreate(int capacity) {
    LFUCache *obj = calloc(1, sizeof(LFUCache));

    obj->capacity = capacity;
    obj->size = 0;
    obj->minFreq = 0;

    return obj;
}

int lFUCacheGet(LFUCache *obj, int key) {
    Node *node = obj->map[key];

    if (node == NULL)
        return -1;

    updateFreq(obj, node);

    return node->value;
}

void lFUCachePut(LFUCache *obj, int key, int value) {
    if (obj->capacity == 0)
        return;

    Node *node = obj->map[key];

    if (node != NULL) {
        node->value = value;
        updateFreq(obj, node);
        return;
    }

    if (obj->size == obj->capacity) {
        List *list = obj->freqList[obj->minFreq];

        Node *lru = list->tail->prev;

        removeNode(lru);
        obj->map[lru->key] = NULL;

        free(lru);
        obj->size--;
    }

    node = createNode(key, value);

    if (obj->freqList[1] == NULL) {
        obj->freqList[1] = createList();
    }

    addFront(obj->freqList[1], node);

    obj->map[key] = node;
    obj->minFreq = 1;
    obj->size++;
}

void lFUCacheFree(LFUCache *obj) {
    for (int i = 0; i < MAX_FREQ; i++) {
        List *list = obj->freqList[i];

        if (list == NULL)
            continue;

        Node *curr = list->head;

        while (curr != NULL) {
            Node *next = curr->next;
            free(curr);
            curr = next;
        }

        free(list);
    }

    free(obj);
}

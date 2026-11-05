#include <stdlib.h>

typedef struct Node {
    int key, value;
    struct Node *prev, *next;
} Node;

typedef struct {
    int capacity, size;
    Node *head, *tail;
    Node *map[10001];
} LRUCache;

void removeNode(Node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void addFront(LRUCache *obj, Node *node) {

    node->next = obj->head->next;
    node->prev = obj->head;

    obj->head->next->prev = node;
    obj->head->next = node;
}

void moveFront(LRUCache *obj, Node *node) {
    removeNode(node);
    addFront(obj, node);
}

LRUCache *lRUCacheCreate(int capacity) {
    LRUCache *obj = calloc(1, sizeof(LRUCache));

    obj->capacity = capacity;

    obj->head = calloc(1, sizeof(Node));
    obj->tail = calloc(1, sizeof(Node));

    obj->head->next = obj->tail;
    obj->tail->prev = obj->head;

    return obj;
}

int lRUCacheGet(LRUCache *obj, int key) {
    Node *node = obj->map[key];

    if (node == NULL)
        return -1;

    moveFront(obj, node);
    return node->value;
}

void lRUCachePut(LRUCache *obj, int key, int value) {
    Node *node = obj->map[key];

    if (node != NULL) {
        node->value = value;
        moveFront(obj, node);
        return;
    }

    if (obj->size == obj->capacity) {
        Node *lru = obj->tail->prev;

        removeNode(lru);
        obj->map[lru->key] = NULL;

        free(lru);
        obj->size--;
    }

    node = malloc(sizeof(Node));
    node->key = key;
    node->value = value;

    addFront(obj, node);
    obj->map[key] = node;
    obj->size++;
}

void lRUCacheFree(LRUCache *obj) {
    Node *curr = obj->head;

    while (curr != NULL) {
        Node *next = curr->next;
        free(curr);
        curr = next;
    }

    free(obj);
}

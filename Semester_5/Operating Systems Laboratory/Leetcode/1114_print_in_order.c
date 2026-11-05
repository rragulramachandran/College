#include <semaphore.h>
#include <stdlib.h>

typedef struct {
    sem_t s1;
    sem_t s2;
} Foo;

Foo *fooCreate() {
    Foo *obj = malloc(sizeof(Foo));

    sem_init(&obj->s1, 0, 0);
    sem_init(&obj->s2, 0, 0);

    return obj;
}

void first(Foo *obj, void (*printFirst)()) {
    printFirst();
    sem_post(&obj->s1);
}

void second(Foo *obj, void (*printSecond)()) {
    sem_wait(&obj->s1);
    printSecond();
    sem_post(&obj->s2);
}

void third(Foo *obj, void (*printThird)()) {
    sem_wait(&obj->s2);
    printThird();
}

void fooFree(Foo *obj) {
    sem_destroy(&obj->s1);
    sem_destroy(&obj->s2);
    free(obj);
}

#include <pthread.h>
#include <stdlib.h>

typedef struct {
    pthread_mutex_t forks[5];
} DiningPhilosophers;

DiningPhilosophers *diningPhilosophersCreate() {
    DiningPhilosophers *obj = malloc(sizeof(DiningPhilosophers));

    for (int i = 0; i < 5; i++) {
        pthread_mutex_init(&obj->forks[i], NULL);
    }

    return obj;
}

void wantsToEat(DiningPhilosophers *obj, int philosopher,

                void (*pickLeftFork)(), void (*pickRightFork)(), void (*eat)(),
                void (*putLeftFork)(), void (*putRightFork)()) {
    int left = philosopher;

    int right = (philosopher + 1) % 5;

    int first = left < right ? left : right;
    int second = left < right ? right : left;

    pthread_mutex_lock(&obj->forks[first]);
    pthread_mutex_lock(&obj->forks[second]);

    pickLeftFork();

    pickRightFork();

    eat();

    putLeftFork();
    putRightFork();

    pthread_mutex_unlock(&obj->forks[second]);
    pthread_mutex_unlock(&obj->forks[first]);
}

void diningPhilosophersFree(DiningPhilosophers *obj) {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_destroy(&obj->forks[i]);
    }

    free(obj);
}

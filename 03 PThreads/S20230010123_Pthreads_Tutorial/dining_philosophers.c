// dining_philosophers.c
// Compile: gcc -pthread -O2 dining_philosophers.c -o dining
// Run:     ./dining 5

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
    int N;
    int idx;
} Philos;

static pthread_mutex_t *forks;
static pthread_mutex_t sem_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t sem_cv = PTHREAD_COND_INITIALIZER;
static int tokens;

static void sem_acquire(void)
{
    pthread_mutex_lock(&sem_lock);
    while (tokens == 0)
        pthread_cond_wait(&sem_cv, &sem_lock);
    tokens--;
    pthread_mutex_unlock(&sem_lock);
}
static void sem_release(void)
{
    pthread_mutex_lock(&sem_lock);
    tokens++;
    pthread_cond_signal(&sem_cv);
    pthread_mutex_unlock(&sem_lock);
}

void think(int idx) { usleep(1000 * (rand() % 30 + 5)); }
void eat(int idx) { usleep(1000 * (rand() % 30 + 5)); }

void *philosopher(void *arg)
{
    Philos *p = (Philos *)arg;
    int left = p->idx;
    int right = (p->idx + 1) % p->N;

    for (int r = 0; r < 5; ++r)
    {
        think(p->idx);

        sem_acquire();

        int first = left < right ? left : right;
        int second = left ^ right ^ first;

        pthread_mutex_lock(&forks[first]);
        pthread_mutex_lock(&forks[second]);

        printf("Philosopher %d is eating (round %d)\n", p->idx, r + 1);
        eat(p->idx);

        pthread_mutex_unlock(&forks[second]);
        pthread_mutex_unlock(&forks[first]);

        sem_release();
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int N = (argc > 1) ? atoi(argv[1]) : 5;
    if (N < 2)
        N = 5;

    forks = calloc(N, sizeof(pthread_mutex_t));
    for (int i = 0; i < N; ++i)
        pthread_mutex_init(&forks[i], NULL);

    pthread_t *ths = malloc(N * sizeof(pthread_t));
    Philos *ps = malloc(N * sizeof(Philos));

    tokens = N - 1;

    for (int i = 0; i < N; ++i)
    {
        ps[i] = (Philos){.N = N, .idx = i};
        pthread_create(&ths[i], NULL, philosopher, &ps[i]);
    }
    for (int i = 0; i < N; ++i)
        pthread_join(ths[i], NULL);

    for (int i = 0; i < N; ++i)
        pthread_mutex_destroy(&forks[i]);
    free(forks);
    free(ths);
    free(ps);
    return 0;
}
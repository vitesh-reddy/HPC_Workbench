// parallel_sum.c
// Compile: gcc -pthread -O2 parallel_sum.c -o psum
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THR 16
#define ARR_SIZE 100000000

long long *data;
long long partial[MAX_THR] = {0};

typedef struct
{
    int s, e, tid;
} TData;

void *sum_part(void *arg)
{
    TData *d = (TData *)arg;
    long long local = 0;
    for (int i = d->s; i < d->e; ++i)
        local += data[i];
    partial[d->tid] = local;
    return NULL;
}

double time_sum(int nthr)
{
    pthread_t th[nthr];
    TData td[nthr];
    clock_t st = clock();
    int chunk = ARR_SIZE / nthr;

    for (int i = 0; i < nthr; ++i)
    {
        td[i].s = i * chunk;
        td[i].e = (i == nthr - 1) ? ARR_SIZE : (i + 1) * chunk;
        td[i].tid = i;
        pthread_create(&th[i], NULL, sum_part, &td[i]);
    }
    long long total = 0;
    for (int i = 0; i < nthr; ++i)
    {
        pthread_join(th[i], NULL);
        total += partial[i];
    }
    clock_t et = clock();
    return (double)(et - st) / CLOCKS_PER_SEC;
}

int main()
{
    data = malloc(ARR_SIZE * sizeof(long long));
    for (int i = 0; i < ARR_SIZE; ++i)
        data[i] = 1;
    for (int t = 1; t <= MAX_THR; ++t)
    {
        double tm = time_sum(t);
        printf("Threads=%d, Time=%lf sec\n", t, tm);
    }
    free(data);
    return 0;
}
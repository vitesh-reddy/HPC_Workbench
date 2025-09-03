// parallel_prefix_sum.c
// Compile: gcc -pthread -O2 parallel_prefix_sum.c -o pscan
// Run:     ./pscan 1000000 8

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct
{
    int *arr, *scan;
    long long *blk_sum;
    size_t s, e;
    int blk_id;
} BlockTask;

static void *phase1_local_scan(void *arg)
{
    BlockTask *t = (BlockTask *)arg;
    if (t->s >= t->e)
    {
        t->blk_sum[t->blk_id] = 0;
        return NULL;
    }

    t->scan[t->s] = t->arr[t->s];
    for (size_t i = t->s + 1; i < t->e; ++i)
        t->scan[i] = t->scan[i - 1] + t->arr[i];

    t->blk_sum[t->blk_id] = t->scan[t->e - 1];
    return NULL;
}

static void *phase3_add_offset(void *arg)
{
    BlockTask *t = (BlockTask *)arg;
    if (t->s >= t->e)
        return NULL;
    long long offset = t->blk_sum[t->blk_id];
    for (size_t i = t->s; i < t->e; ++i)
        t->scan[i] += offset;
    return NULL;
}

int main(int argc, char **argv)
{
    size_t N = (argc > 1) ? strtoull(argv[1], NULL, 10) : 32;
    int T = (argc > 2) ? atoi(argv[2]) : 4;
    if (T < 1)
        T = 1;

    int *arr = malloc(N * sizeof(int));
    int *scan = malloc(N * sizeof(int));
    for (size_t i = 0; i < N; ++i)
        arr[i] = 1;

    pthread_t *th = malloc(T * sizeof(pthread_t));
    BlockTask *tasks = malloc(T * sizeof(BlockTask));
    long long *blk_sum = calloc(T, sizeof(long long));
    long long *blk_offset = calloc(T, sizeof(long long));

    size_t chunk = (N + T - 1) / T;
    int blocks = T;

    for (int b = 0; b < blocks; ++b)
    {
        size_t s = b * chunk;
        size_t e = (s + chunk > N) ? N : s + chunk;
        tasks[b] = (BlockTask){.arr = arr, .scan = scan, .blk_sum = blk_sum, .s = s, .e = e, .blk_id = b};
        pthread_create(&th[b], NULL, phase1_local_scan, &tasks[b]);
    }
    for (int b = 0; b < blocks; ++b)
        pthread_join(th[b], NULL);

    blk_offset[0] = 0;
    for (int b = 1; b < blocks; ++b)
        blk_offset[b] = blk_offset[b - 1] + blk_sum[b - 1];

    for (int b = 0; b < blocks; ++b)
        blk_sum[b] = blk_offset[b];

    for (int b = 1; b < blocks; ++b)
        pthread_create(&th[b], NULL, phase3_add_offset, &tasks[b]);
    for (int b = 1; b < blocks; ++b)
        pthread_join(th[b], NULL);

    printf("N=%zu, T=%d, final prefix sum=%d\n",
           N, T, scan[N - 1]);

    free(arr);
    free(scan);
    free(th);
    free(tasks);
    free(blk_sum);
    free(blk_offset);
    return 0;
}
// pthread_mergesort.c
// Compile: gcc -pthread -O2 pthread_mergesort.c -o pmsort
// Run:     ./pmsort 1000000 8

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int *arr, *tmp;
    int l, r;
    int d, maxd;
} Task;

static inline void ins_sort(int *arr, int l, int r)
{
    for (int i = l + 1; i < r; ++i)
    {
        int v = arr[i], j = i - 1;
        while (j >= l && arr[j] > v)
        {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = v;
    }
}

static void merge(int *arr, int *tmp, int l, int m, int r)
{
    int i = l, j = m, k = l;
    while (i < m && j < r)
        tmp[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
    while (i < m)
        tmp[k++] = arr[i++];
    while (j < r)
        tmp[k++] = arr[j++];
    for (i = l; i < r; ++i)
        arr[i] = tmp[i];
}

static void *merge_sort_task(void *arg);

static void spawn_or_run(Task *L, Task *R)
{
    if (L->d < L->maxd)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, merge_sort_task, R);
        merge_sort_task(L);
        pthread_join(tid, NULL);
    }
    else
    {
        merge_sort_task(L);
        merge_sort_task(R);
    }
}

static void *merge_sort_task(void *arg)
{
    Task *T = (Task *)arg;
    int n = T->r - T->l;
    if (n <= 32)
    {
        ins_sort(T->arr, T->l, T->r);
        return NULL;
    }
    int mid = T->l + n / 2;

    Task TL = *T;
    TL.r = mid;
    TL.d = T->d + 1;
    Task TR = *T;
    TR.l = mid;
    TR.d = T->d + 1;

    spawn_or_run(&TL, &TR);
    merge(T->arr, T->tmp, T->l, mid, T->r);
    return NULL;
}

int main(int argc, char **argv)
{
    int N = (argc > 1) ? atoi(argv[1]) : 100000;
    int nthreads = (argc > 2) ? atoi(argv[2]) : 8;

    int d = 0, t = nthreads;
    while (t > 1)
    {
        d++;
        t >>= 1;
    }

    int *arr = malloc(N * sizeof(int));
    int *tmp = malloc(N * sizeof(int));
    srand(42);
    for (int i = 0; i < N; ++i)
        arr[i] = rand() % 1000000;

    Task root = {.arr = arr, .tmp = tmp, .l = 0, .r = N, .d = 0, .maxd = d};
    merge_sort_task(&root);

    // verify
    int sorted = 1;
    for (int i = 1; i < N; ++i)
        if (arr[i - 1] > arr[i])
        {
            sorted = 0;
            break;
        }
    printf("Sorted? %s\n", sorted ? "YES" : "NO");

    free(arr);
    free(tmp);
    return 0;
}
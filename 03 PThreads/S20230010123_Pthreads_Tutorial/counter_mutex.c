// counter_mutex.c
// Compile: gcc -pthread -O2 counter_mutex.c -o counter
#include <stdio.h>
#include <pthread.h>

int cnt = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *inc(void *a)
{
    for (int i = 0; i < 100000; ++i)
    {
        pthread_mutex_lock(&lock);
        cnt++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2;
    pthread_create(&t1, NULL, inc, NULL);
    pthread_create(&t2, NULL, inc, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Final counter value: %d\n", cnt);
    return 0;
}
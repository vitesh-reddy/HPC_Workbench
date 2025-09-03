// lock_ordering.c
// Compile: gcc -pthread -O2 lock_ordering.c -o locks
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t M1=PTHREAD_MUTEX_INITIALIZER, M2=PTHREAD_MUTEX_INITIALIZER;

void* thread1(void* a){
    pthread_mutex_lock(&M1);
    pthread_mutex_lock(&M2);
    printf("Thread 1 executing\n");
    pthread_mutex_unlock(&M2);
    pthread_mutex_unlock(&M1);
    return NULL;
}
void* thread2(void* a){
    pthread_mutex_lock(&M1);
    pthread_mutex_lock(&M2);
    printf("Thread 2 executing\n");
    pthread_mutex_unlock(&M2);
    pthread_mutex_unlock(&M1);
    return NULL;
}

int main(){
    pthread_t t1, t2;
    pthread_create(&t1,NULL,thread1,NULL);
    pthread_create(&t2,NULL,thread2,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    return 0;
}
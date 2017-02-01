//
//  main.c
//  semaphoreTrials
//
//  Created by Ahmed Moussa on 1/30/17.
//  Copyright © 2017 Ahmed Moussa. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */

#include <time.h>
#include <sched.h>


/*
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
sem_t * sem;
int threads = 8;


void* func1(void * theOneVariable){
    sem_wait(sem);
    int semValue = -11;
    sem_getvalue(sem, &semValue);
    printf("%d\n", semValue);
    
    int i = *(int*)theOneVariable;
    
    printf("this is func 1, the one variable is: %d\n", i);
    return NULL;
    //return theOneVariable;
}

int main(){
    printf("why?");
    sem = (sem_t *)malloc(sizeof(sem_t));
    //sem = sem_open("name", 0, O_CREAT, 0);
    
    sem_init(sem, 0, 0);
    int semValue = -11;
    sem_getvalue(sem, &semValue);
    printf("%d\n", semValue);
    sem_wait(sem);
    sem_getvalue(sem, &semValue);
    printf("%d\n", semValue);
    sem_post(sem);
    sem_getvalue(sem, &semValue);
    printf("%d\n", semValue);
    
    
    pthread_t thread_table[threads];
    int i = 0;
    for(i = 0; i < threads; i++) {
        pthread_create(&(thread_table[i]), NULL, func1, &i);
        pthread_join((thread_table[i]), NULL);
    }
    i = 10;
    for(i = 0; i < threads; i++) {
        sem_post(sem);
    }
}
*/













//pthread_mutex_t join_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ready_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void thread_function (void *param) {
    pthread_mutex_lock(&ready_mut);
    
    pthread_cond_wait(&cond, &ready_mut);
    pthread_mutex_unlock(&ready_mut);
    
    /* Thread work here */
}

enum { threads = 7 };
int i;
int ready;

void* func1(void * theOneVariable){
    pthread_mutex_lock(&ready_mut);
    ready++;
    pthread_cond_wait(&cond, &ready_mut);
    pthread_mutex_unlock(&ready_mut);
    
    int j = *(int*)theOneVariable;
    
    printf("this is func 1, the one variable is: %d\n", i);
    return NULL;
    //return theOneVariable;
}
int func2(int theOneVariable){
    return theOneVariable;
}

int main() {
    
    printf("sup world\n");
    
    pthread_t thread_table[threads];
    
    for(i = 0; i < threads; i++) {
        pthread_create(&(thread_table[i]), NULL, func1, &i);
        //pthread_join((thread_table[i]), NULL);
    }
    i = 10;
    /*
    for(i = 0; i < threads; i++) {
        pthread_join((thread_table[i]), NULL);
    }*/
    /*Wait for all threads to be queued */
    
    
    /*
    printf("before sleep and i = %d\n", i);
    struct timespec forsleep;
    forsleep.tv_sec = 1;
    forsleep.tv_nsec = 500;
    nanosleep(&forsleep, NULL);
    printf("after sleep and i = %d\n", i);
     */
    //instead of sleep?
    
    while(ready != threads);
    
    pthread_cond_broadcast(&cond);
    printf("before sleep and i = %d\n", i);
    struct timespec forsleep;
    forsleep.tv_sec = 1;
    forsleep.tv_nsec = 500;
    nanosleep(&forsleep, NULL);
    printf("after sleep and i = %d\n", i);
    
    
}

//
//  main.c
//  pthreads
//
//  Created by Ahmed Moussa on 1/18/17.
//  Copyright © 2017 Ahmed Moussa. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include<signal.h>
#include<unistd.h>
#include <errno.h>
#include <sys/time.h>

pthread_t one;
pthread_t two;


pthread_mutex_t mutex1;

int what = 0;


void * func1(){
    
    //pSigHandler(SIGTSTP);
    
    struct timespec period;
    //printf("before sleep and signal\n");
    period.tv_sec = 1;
    period.tv_nsec = 500;
    int pretendvar = 0;
    
    int sig, err;
    sigset_t newmask, set;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGTERM);
    sigaddset(&newmask, SIGALRM);
    
    int i = 0;
    for(i=0; i < 999999; i++){
        //pretendvar =
        pthread_sigmask(SIG_UNBLOCK, &set, NULL );
        printf("func1 is sleeping... \n");
        if(nanosleep(&period, NULL) != 0){
            printf("should handle termination then break");
        }
        printf("func1 woke up\n");
        pthread_sigmask(SIG_BLOCK, &set, NULL );
        
        printf("it should never terminate between this... \n");
        nanosleep(&period, NULL);
        printf("and this!\n");
        what++;
        printf("ONE loop number: %d and what is: %d\n", i, what);
        
        
        /*
         printf("waiting for signal \n");
         err= sigwait(&newmask, &sig);
         if (err ||  sig != SIGALRM){
         printf("expected signal not genearated \n");
         abort();
         }
         else{
         printf("Alarm generated : OK\n");
         }
         */
    }
    return NULL;
};

void * func2(){
    //pthread_mutex_lock(&mutex1);
    struct timespec period;
    //printf("before sleep and signal\n");
    period.tv_sec = 1;
    period.tv_nsec = 500;
    
    int i = 0;
    
    for(i = 0; i < 99999999999100; i++){
        nanosleep(&period, NULL);
        what++;
        printf("TWO loop number: %d and what is: %d\n", i, what);
        
    }
    //pthread_mutex_unlock(&mutex1);
    return NULL;
};


static void catcher(int signo){
    //switch (signo) {
    //   case SIGTSTP:
    //      printf("TSTP\n");
    //    fflush(stdout);
    //
    // pthread_kill(one, 0);
    //break;
    //}
    
    printf("in the signal handler... ");
    if(signo == SIGTERM){
        printf("the signal was SIGTERM \n");
    }
    else{
        printf("the signal was NOT SIGTERM \n");
    }
}

int main(int argc, const char * argv[]) {
    int x;
    pthread_attr_t attr1;
    struct sched_param param1;
    pthread_attr_t attr2;
    struct sched_param param2;
    
    
    x = pthread_attr_init(&attr1);
    x = pthread_attr_init(&attr2);
    
    param1.sched_priority = 99;
    param2.sched_priority = 0;
    
    x = pthread_attr_setschedparam(&attr1, &param1);
    x = pthread_attr_setschedparam(&attr2, &param2);
    
    pthread_setschedparam(one, SCHED_FIFO, &param1);
    pthread_setschedparam(two, SCHED_FIFO, &param2);
    
    if(pthread_mutex_init(&mutex1, NULL)){
        printf("Error initiating mutex\n");
    }
    if(pthread_create(&one, &attr1, &func1, NULL)){
        printf("Error1\n");
    }
    if(pthread_create(&two, &attr2, &func2, NULL)){
        printf("Error2\n");
    }
    
    
    
    
    //pthread_join(one, NULL);
    //pthread_join(two, NULL);
    
    
    struct timespec period;
    printf("before sleep and signal\n");
    period.tv_sec = 5;
    period.tv_nsec = 500;
    
    nanosleep(&period, NULL);
    
    printf("after 1st sleep but before signal\n");
    
    struct sigaction sigAction;
    sigAction.sa_handler = catcher;
    sigaction(SIGTERM, &sigAction, NULL);
    
    alarm(1);
    //setitt
    //pSigHandler(SIGTSTP);
    printf("after signal but before 2nd sleep\n");
    
    nanosleep(&period, NULL);
    
    printf("after sleep and signal\n");
    
    while(1);
    
    printf("Hello, World!\n");
    return 0;
}

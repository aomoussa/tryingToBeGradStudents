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

pthread_mutex_t mutex1;

int what = 0;

void * func1(){
    for(int i = 0; i < 100; i++){
        //what++;
        //printf("ONE loop number: %d and what is: %d\n", i, what);
        
        pthread_mutex_lock(&mutex1);
        while(what>50 && what<90){
            what++;
            i++;
            printf("ONE loop number: %d and what is: %d\n", i, what);
        }
        pthread_mutex_unlock(&mutex1);
        
    }
    return NULL;
};

void * func2(){
    pthread_mutex_lock(&mutex1);
    for(int i = 0; i < 100; i++){
        
        what++;
        printf("TWO loop number: %d and what is: %d\n", i, what);
        
    }
    pthread_mutex_unlock(&mutex1);
    return NULL;
};


int main(int argc, const char * argv[]) {
    int x;
    pthread_attr_t attr1;
    struct sched_param param1;
    pthread_t one;
    pthread_t two;
    
    x = pthread_attr_init(&attr1);
    
    param1.sched_priority = 0;
    
    x = pthread_attr_setschedparam(&attr1, &param1);
    
    if(pthread_mutex_init(&mutex1, NULL)){
        printf("Error initiating mutex\n");
    }
    if(pthread_create(&one, NULL, &func1, NULL)){
        printf("Error1\n");
    }
    if(pthread_create(&two, NULL, &func2, NULL)){
        printf("Error2\n");
    }
    

    
    pthread_join(one, NULL);
    pthread_join(two, NULL);
    
    printf("Hello, World!\n");
    return 0;
}

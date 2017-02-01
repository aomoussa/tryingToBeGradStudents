
//  main.c
//  ass1_v2
//
//  Created by Kendall Hickie on 1/20/17.
//  Copyright © 2017 Kendall Hickie. All rights reserved.
//
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

pthread_mutex_t ready_mut = PTHREAD_MUTEX_INITIALIZER;//for ready pthreads count accessing
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //to signal when all threads are ready

pthread_cond_t condEvent[5] = {PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER};

pthread_mutex_t event_mut[5] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};

int readyTasksEvent[5];

int numTasksEvent[5];


pthread_mutex_t mutexArray[10];

int readyTasksCount;
int numTasks;
int totalTaskTime;
int waitFlag;

typedef struct{
    char type;
    int priority;
    int event;
    long period;
	int bodyLength;
    char ** body;
}Task;


Task * parseLine(char* line, Task * taskArray){
    
    char* tempBody[200];
    char* element;
	char temp = ' ';

	taskArray = (Task *)malloc(sizeof(Task));
    
    printf ("Splitting line \"%s\" into elements:\n",line);
    
    element = strtok(line," ");
    printf("Element is:%s\n", element);
    taskArray->type = element[0];
   	printf("The type is:%c\n", taskArray->type);
    
    if(taskArray->type == 'P'){
        //task is periodic
        printf("This task is periodic.\n");

        element = strtok (NULL, " ");
        taskArray->priority = atoi(element);
        printf("The priority is:%d\n", taskArray->priority);
        
        taskArray->event = -1;
        printf("The event is:%d\n", taskArray->event);

        
        element = strtok (NULL, " ");
        taskArray->period = atoi(element);
        printf("The period is:%d\n", taskArray->period);
        
        //THIS PART ISNT VERY GOOD.......................
        int i = 0;
        while (element != NULL && i < 200)
        {
            element = strtok (NULL, " ");
            printf ("The element is: %s\n",element);
            tempBody[i] = element;
            i++;
        }

        if(i > 0){
 			taskArray->bodyLength = i - 1;
			printf("body length %d\n", taskArray->bodyLength);
            taskArray->body = malloc((i-1)*sizeof(char*));   //should be
            int j = 0;
            for(j = 0; j < taskArray->bodyLength; j++){
                taskArray->body[j] = malloc(sizeof(tempBody[j]));
                strcpy(taskArray->body[j], (const char *)tempBody[j]);
            }
        }
		else{
			taskArray->bodyLength = 0;
		}

    }
    else if(taskArray->type == 'A'){
        //task if aperiodic
        printf("This task is aperiodic.\n");
        
        element = strtok (NULL, " ");
        taskArray->priority = atoi(element);
        printf("The priority is:%d\n", taskArray->priority);
        
        element = strtok (NULL, " ");
        taskArray->event = atoi(element);
        printf("The event is:%d\n", taskArray->event);
        
        //element = strtok (NULL, " ");
        taskArray->period = 0;
        printf("The period is:%d\n", taskArray->period);
        
        //THIS PART ISNT VERY GOOD.......................
        int i = 0;
        while (element != NULL && i < 200)
        {
            element = strtok (NULL, " ");
            printf ("The element is: %s\n",element);
            tempBody[i] = element;
            i++;
        }
        printf("%d\n", i);
        
        if(i > 0){
			taskArray->bodyLength = i - 1;
			printf("body length %d\n", taskArray->bodyLength);
            taskArray->body = malloc((i-1)*sizeof(char*));   //should be
            int j = 0;
            for(j = 0; j < taskArray->bodyLength; j++){
                taskArray->body[j] = malloc(sizeof(tempBody[j]));
                strcpy(taskArray->body[j], (const char *)tempBody[j]);
            }
        }
        
    }
    
    return taskArray;
}


Task ** getInput(FILE * ptr){
    
    //read the first line
    //determine how many tasks there will be
    //determine the entire time for all tasks
    //read in for the number of lines required (aka the number of taks)
    //store their information inside of the array of structs

    char * input;
    size_t numOfBytes = 0;
    Task ** taskArray;
    
    if(getline(&input, &numOfBytes, ptr)){
        printf("The input was correct!.\n");
        
        numTasks = atoi(strtok(input," ")); //get the number of tasks
        totalTaskTime = atoi(strtok (NULL, " "));   //get the total task time
		printf("numTasks - %d\n", numTasks);
		printf("taskTime - %d\n", totalTaskTime);        

        taskArray = (Task **)malloc(numTasks*sizeof(Task *));      

        //while there is still input, get the next line(s)
        int taskLineCount = 0;
        while(taskLineCount < numTasks){
            if(getline(&input, &numOfBytes, ptr)){
                printf("The input was correct.\n");
				printf("allocated");
                taskArray[taskLineCount] = parseLine(input, taskArray[taskLineCount]);
                printf("This is the type of the task (Pass by reference): %c\n", taskArray[taskLineCount]->type);
                printf("This is the event of the task (Pass by reference): %d\n", taskArray[taskLineCount]->event);
                printf("This is the priority of the task (Pass by reference): %d\n", taskArray[taskLineCount]->priority);
                printf("This is the period of the task (Pass by reference): %d\n", taskArray[taskLineCount]->period);

                taskLineCount++;
            }
        }
        return taskArray;
    }
    else{
        printf("The input was incorrect.\n");
    }
    return NULL;

 
}

static const char *const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
};

void triggerEvent(int eventID){
    while(readyTasksEvent[eventID] != numTasksEvent[eventID]);//busy loop until all threads are ready
    //printf("after busy loop but before broadcast\n");
    pthread_cond_broadcast(&condEvent[eventID]);//broadcast signal to start when after busy loop exits
    //printf("after busy loop and broadcast\n");
    //temporary sleep stuff until we figure out how to stop all tasks
    
    printf("event number %d was triggered, i'm just gonna chill for a bit until we know wtf is going on\n", eventID);
    //.struct timespec forsleep;
    //forsleep.tv_sec = 3;
    //forsleep.tv_nsec = 500;
    //nanosleep(&forsleep, NULL);
    
}

void * keyboardListen(void * ptr){
    const char *dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
    struct input_event ev;
    ssize_t n;
    int fd;

    fd = open(dev, O_RDONLY);
    if (fd == -1) {
        printf("Cannot open %s: %s.\n", dev, strerror(errno));
        return NULL;
    }
    while (1) {
        n = read(fd, &ev, sizeof ev);
        if (n == (ssize_t)-1){
            if (errno == EINTR)
                continue;
            else
                break;
      	} 
		else if(n != sizeof(ev)){
            errno = EIO;
            break;
    	}
		if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
			if(ev.code == 2 || ev.code == 3 || ev.code == 4 || ev.code == 5 || ev.code == 6){
                triggerEvent(ev.code - 2);
				printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
			}
    }

	fflush(stdout);
	fprintf(stderr, "%s.\n", strerror(errno));
	return NULL;
}


void compute(int x){
    
    int i, j=0;
    for (i = 0; i < x; i++)
    {
        j = j + i;
    }
    printf("compute\n");
}

void* periodicTask(void * periodicTask){

    /*
	waitFlag = waitFlag - 1;
	while(waitFlag != 0){
		;
	}*/
    pthread_mutex_lock(&ready_mut);
    readyTasksCount++;
    pthread_cond_wait(&cond, &ready_mut);
    pthread_mutex_unlock(&ready_mut);
    

	Task * newTask = (Task *)periodicTask;
    //INIT
    //WAIT FOR ACTIVATION
    printf("Periodic Task\n");
    printf("%c\n", newTask->type);
    int i = 0;

	struct timespec next, period;
	printf("periodic task period %d\n",newTask->period);
	period.tv_nsec = (long)newTask->period*1000000;
	printf("period.tv_nsec %lu\n", period.tv_nsec);
    while(i < newTask->bodyLength){
		clock_gettime(CLOCK_MONOTONIC, &next); 
        if(atoi(newTask->body[i])){
            compute(atoi(newTask->body[i]));
            printf("%d\n", atoi(newTask->body[i]));
        }
        else{
            char mutexChar;
            int mutexNumber;
            if(newTask->body[i][0] == 'L'){
                mutexChar = newTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
                printf("%d\n", mutexNumber);
                //Lock the correct mutex
				pthread_mutex_lock(&mutexArray[mutexNumber-1]);
                //LockMutex(mutexArray[mutexNumber])
                
            }
            else if(newTask->body[i][0] == 'U'){
                mutexChar = newTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
                printf("%d\n", mutexNumber);
                //unlock the correct mutex
				pthread_mutex_unlock(&mutexArray[mutexNumber-1]);
                //UnlockMutex(mutexArray[mutexNumber])
            }
        }

		printf("next %lu\n", next.tv_nsec);
		next.tv_nsec = next.tv_nsec + period.tv_nsec; 
		printf("next + period %lu\n", next.tv_nsec);
		if(next.tv_nsec >= 1000000000L){		//if there is overflow...
			next.tv_nsec -= 1000000000L;       	// handle "carry" to seconds field
    		next.tv_sec++;						//increase seconds
		}

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);

		clock_gettime(CLOCK_MONOTONIC, &next);
		printf("next after sleep %lu\n", next.tv_nsec);
        i++;
    }
    //not sure what wait for activation is
    return NULL;

/*
    < local variables >
    initialization() and wait_for_activation();
    while (condition) {
        <task body>
        wait_for_period();
    }
*/
}

void* aperiodicTask(void * aperiodicTask){

    printf("APERIODIC\n");
    /*
	waitFlag = waitFlag - 1;
	while(waitFlag != 0){
		;
	}*/

    pthread_mutex_lock(&ready_mut);
    readyTasksCount++;
    pthread_cond_wait(&cond, &ready_mut);
    pthread_mutex_unlock(&ready_mut);

    printf("APERIODIC2\n");

    Task * newTask = (Task *)aperiodicTask;

    int eventID = newTask->event - 1;
    pthread_mutex_lock(&event_mut[eventID]);
    readyTasksEvent[eventID]++;
    pthread_cond_wait(&condEvent[eventID], &event_mut[eventID]);
    pthread_mutex_unlock(&event_mut[eventID]);
    
	printf("here");
    //-------------- ----------- --------- pasted from periodic task ----------- ----------- begins
	
    printf("%c\n", newTask->type);
    int i = 0;
    
    while(i < newTask->bodyLength){
        if(atoi(newTask->body[i])){
            compute(atoi(newTask->body[i]));
            printf("%d\n", atoi(newTask->body[i]));
        }
        else{
            char mutexChar;
            int mutexNumber;
            if(newTask->body[i][0] == 'L'){
                mutexChar = newTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
                printf("%d\n", mutexNumber);
                //Lock the correct mutex
                pthread_mutex_lock(&mutexArray[mutexNumber-1]);
                //LockMutex(mutexArray[mutexNumber])
                
            }
            else if(newTask->body[i][0] == 'U'){
                mutexChar = newTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
                printf("%d\n", mutexNumber);
                //unlock the correct mutex
                pthread_mutex_unlock(&mutexArray[mutexNumber-1]);
                //UnlockMutex(mutexArray[mutexNumber])
            }
        }

        i++;
    }
    //not sure what wait for activation is
    return NULL;
//-------------- ----------- --------- pasted from periodic task ----------- ----------- ends
/*
    < local variables >
    Initialization() and wait_for_activation();
    wait_for_event();
    while (condition) {
        <task body>
        wait_for_event();
    }
*/
}



int main(int argc, const char * argv[]) {
    FILE *filePointer;
    
    filePointer = fopen("test.txt", "r");
    if(filePointer == NULL){
        printf("couldn't open file");
        return -1;
    }

    printf("Hello, World! hey Ahmed is here\n");
    Task ** taskList;
	
 
    taskList = getInput(filePointer);
    if(taskList != NULL){
        printf("back from getInput, it worked!\n");
        printf("These are the tasks: \n");
        printf("Task 1: \n");
        printf("\tType: %c\n", taskList[0]->type);
        printf("\tPriority: %d\n", taskList[0]->priority);
        printf("\tEvent: %d\n", taskList[0]->event);
        printf("\tPeriod: %d\n", taskList[0]->period);
        printf("\tBody: \n");
        
        int i = 0;
        while(i < taskList[0]->bodyLength){
            printf("\t\t%s\n", taskList[0]->body[i]);
            i++;
        }
        
        printf("Task 2: \n");
        printf("\tType: %c\n", taskList[1]->type);
        printf("\tPriority: %d\n", taskList[1]->priority);
        printf("\tEvent: %d\n", taskList[1]->event);
        printf("\tPeriod: %d\n", taskList[1]->period);
        printf("\tBody: \n");
        
        int j = 0;
        while(j < taskList[1]->bodyLength){
            printf("\t\t%s\n", taskList[1]->body[j]);
            j++;
        }
        
		/*
			pthread_mutex_t my_mutex;
			pthread_mutexattr_t my_mutex_attr;
			pthread_mutexattr_init(&my_mutex_attr);
			pthread_mutexattr_setprotocol(&my_mutex_attr, PTHREAD_PRIO_INHERIT);
			pthread_mutex_init(&my_mutex, &my_mutex_attr);
			pthread_mutexattr_destroy(&my_mutex_attr);
		*/
		waitFlag = numTasks;

		pthread_t threadID[numTasks];

		pthread_mutexattr_t piMutexAttr;
		pthread_mutexattr_init(&piMutexAttr);
		pthread_mutexattr_setprotocol(&piMutexAttr, PTHREAD_PRIO_INHERIT);

		int mutexCount;
		for(mutexCount = 0; mutexCount < 10; mutexCount++){
			//init mutex and set protocol to priority inheritance
			pthread_mutex_init(&mutexArray[mutexCount], &piMutexAttr);
		}
        int e = 0;
    /*    for(e=0; e<5; e++){
            numTasksEvent[e] = 0;
            readyTasksEvent[e] = 0;
       }
*/
		pthread_attr_t kbdAttr;
		struct sched_param kbdParam;
		pthread_attr_init(&kbdAttr);
		kbdParam.sched_priority = SCHED_FIFO;
		pthread_attr_setschedparam(&kbdAttr, &kbdParam);
		pthread_t id;

		pthread_create(&id, &kbdAttr, &keyboardListen, (void *)NULL);
		printf("PERIODIC THREAD CREATED\n");


		pthread_attr_t pthreadAttr;
    	struct sched_param pthreadParam;
		pthread_attr_init(&pthreadAttr);
    	pthreadParam.sched_priority = SCHED_FIFO;
    	pthread_attr_setschedparam(&pthreadAttr, &pthreadParam);


		//create mutiple threads
		for(i = 0; i < numTasks; i++){
			if(taskList[i]->type == 'P'){
				pthread_create(&threadID[i], &pthreadAttr, &periodicTask, (void *)taskList[i]);
				printf("PERIODIC THREAD %d CREATED\n", i);
			}
			else if(taskList[i]->type == 'A'){
                numTasksEvent[taskList[i]->event - 1]++;
				pthread_create(&threadID[i], &pthreadAttr, &aperiodicTask, (void *)taskList[i]);
				printf("APERIODIC THREAD %d CREATED\n", i);
			}
			
		}

        //printf("before busy loop/n");
        while(readyTasksCount != numTasks);//busy loop until all threads are ready
        //printf("after busy loop but before broadcast\n");
        pthread_cond_broadcast(&cond);//broadcast signal to start when after busy loop exits
        //printf("after busy loop and broadcast\n");
        //temporary sleep stuff until we figure out how to stop all tasks
        //struct timespec forsleep;
        //forsleep.tv_sec = 5;
        //forsleep.tv_nsec = 500;
        //nanosleep(&forsleep, NULL);

		//running = 1

		//broadcast signal to end

        //we're not gonna be using join so might as well delete this
        
		for(i = 0; i < numTasks; i++){
			pthread_join(threadID[i], NULL);
		}
        printf("THE END!!!\n");
        //periodicTask(taskList[0]);

    }
    else{
        printf("someone messed up\n");
    }
    
    //need to run tasks based on p or a in their own thread.... with all 10 mutexes
    //probably in a loop, for all tasks
        //do we need to change the attributes before we create the thread?
            //probably
    //so...
    //while(size of tasks && period for entire program)
        //init attribute
        //task[i]-> set priority
        //create_thread()
        //next task
    

	//not period for aperiodic
		//it is the computations 

    
    //while(size of tasks)
        //join threads
    
    
    return 0;
}

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
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

//global mutex to signal when all threads have been created (barrier synchronization
pthread_mutex_t ready_mut = PTHREAD_MUTEX_INITIALIZER;//for ready pthreads count accessing
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //to signal when all threads are ready

//pthread contidion event
pthread_cond_t condEvent[5] = {PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER};

//pthread mutex event
pthread_mutex_t event_mut[5] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};

//array to hold number of tasks per event
int readyTasksEvent[5];

//number of tasks per event
int numTasksEvent[5];

//mutex array for input
pthread_mutex_t mutexArray[10];

//global variables
int readyTasksCount;
int numTasks;
int totalTaskTime;

int flagToTerminate;



//struct to hold task data
typedef struct{
    char type;
    int priority;
    int event;
    long period;
	int bodyLength;
    char ** body;
}Task;


//function that parses each line and puts the line information into the struct
Task * parseLine(char* line, Task * taskArray){
    
    char* tempBody[200];
    char* element;
	char temp = ' ';

	taskArray = (Task *)malloc(sizeof(Task));	//holds the task array that was passed in
    
    element = strtok(line," ");		
    taskArray->type = element[0];	//store the first character of the line into the struct->type

    //if the type is a P the task is periodic, else if the type is A the task is aperiodic
    if(taskArray->type == 'P'){
        //task is periodic

        element = strtok (NULL, " ");
        taskArray->priority = atoi(element);	//store the second value in the line into struct->prioirity
        
        taskArray->event = -1;			//periodic task does not have an even so store as -1
        
        element = strtok (NULL, " ");
        taskArray->period = atoi(element);		//store the third value in the line into struct->period
        
        int i = 0;
        while (element != NULL && i < 200){			//hold the rest of the line in a temp body
            element = strtok (NULL, " ");
            tempBody[i] = element;
            i++;
        }

        if(i > 0){			//store the rest of the values in the line into struct->body
 			taskArray->bodyLength = i - 1;					//store the body length
            taskArray->body = malloc((i-1)*sizeof(char*));   
            int j = 0;
            for(j = 0; j < taskArray->bodyLength; j++){
                taskArray->body[j] = malloc(sizeof(tempBody[j]));
                strcpy(taskArray->body[j], (const char *)tempBody[j]);	//put temp body into the struct->taskbody
            }
        }
		else{
			taskArray->bodyLength = 0;
		}

    }
    else if(taskArray->type == 'A'){
        //task if aperiodic
        
        element = strtok (NULL, " ");	
        taskArray->priority = atoi(element);	//store the second value in the line into struct->prioirity
        
        element = strtok (NULL, " ");
        taskArray->event = atoi(element);		//store the third value in the line into struct->event

        taskArray->period = 0;					//an aperiodic task does not have a period so struct->period = 0
        
        int i = 0;
        while (element != NULL && i < 200){		//store the rest of the line into a temp body
            element = strtok (NULL, " ");
            tempBody[i] = element;
            i++;
        }
        
        if(i > 0){					//store the rest of the values in the line into struct->body
			taskArray->bodyLength = i - 1;		//store the length of the body in struct->body
            taskArray->body = malloc((i-1)*sizeof(char*));   
            int j = 0;
            for(j = 0; j < taskArray->bodyLength; j++){
                taskArray->body[j] = malloc(sizeof(tempBody[j]));
                strcpy(taskArray->body[j], (const char *)tempBody[j]);		//store the temp body into struct->body
            }
        }
        
    }
    
    return taskArray;		//return the instance of the task array
}

//function that gets the lines in the file for the number of tasks specified
Task ** getInput(FILE * ptr){

    char * input;
    size_t numOfBytes = 0;
    Task ** taskArray;
    
    if(getline(&input, &numOfBytes, ptr)){	//get the first line
  
        numTasks = atoi(strtok(input," ")); 		//get the number of tasks
        totalTaskTime = atoi(strtok (NULL, " "));   //get the total task time      

        taskArray = (Task **)malloc(numTasks*sizeof(Task *));      

        //while there is still input, get the next line(s)
        int taskLineCount = 0;
        while(taskLineCount < numTasks){
            if(getline(&input, &numOfBytes, ptr)){
                taskArray[taskLineCount] = parseLine(input, taskArray[taskLineCount]); //call parse line to store the data into the struct
                taskLineCount++;
            }
        }
        return taskArray;	//return the entire array of tasks
    }
    else{
        printf("The input file was not the correct format.\n");
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
    
    printf("event number %d was triggered, i'm just gonna chill for a bit until we know is going on\n", eventID);
    
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
		if (ev.type == EV_KEY && ev.value == 0 )
			if(ev.code == 2 || ev.code == 3 || ev.code == 4 || ev.code == 5 || ev.code == 6){
                triggerEvent(ev.code - 2);
				printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
			}
    }

	fflush(stdout);
	fprintf(stderr, "%s.\n", strerror(errno));
	return NULL;
}

//compute function that simulates the iteration
void compute(int x){
    
    int i, j=0;
    for (i = 0; i < x; i++)
    {
        j = j + i;
    }
}

//periodic task function
void* periodicTask(void * periodicTask){

    printf("periodic task\n");
	printf("%lu\n", pthread_self());

	//barrier synchronization
    pthread_mutex_lock(&ready_mut);
    int thisTaskID = readyTasksCount++;
    pthread_cond_wait(&cond, &ready_mut);
    pthread_mutex_unlock(&ready_mut);
    
    
    //-------- --------- --------- initiations necessary for masking/unmasking -------- -------- begin
    int cancelStateResponse;
    //-------- --------- --------- initiations necessary for masking/unmasking -------- -------- end
    
	Task * newTask = (Task *)periodicTask;

	struct timespec next, period;
	period.tv_nsec = (long)newTask->period*1000000;

	//run the task
    int i = 0;
    while(i < newTask->bodyLength){
        cancelStateResponse = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        if (cancelStateResponse != 0)
        {
            printf("setting cancel state return non-zero: %d", cancelStateResponse);
        }
        
        if(flagToTerminate == 1){
            break;
        }
		clock_gettime(CLOCK_MONOTONIC, &next); 	//get the initital time
        if(atoi(newTask->body[i])){				//if the next task is an iteration
            compute(atoi(newTask->body[i]));		//call the iteration function
            printf("task %d computed %d iterations\n",thisTaskID,  atoi(newTask->body[i]));
        }
        else{									//the task is a mutex
            char mutexChar;
            int mutexNumber;
            if(newTask->body[i][0] == 'L'){			//lock the mutex
                mutexChar = newTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
				pthread_mutex_lock(&mutexArray[mutexNumber-1]);		 //Lock the correct mutex
				printf("mutex %d locked in periodic task %d\n", mutexNumber, thisTaskID);
            }
            else if(newTask->body[i][0] == 'U'){		//unlock the mutex
                mutexChar = newTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
				pthread_mutex_unlock(&mutexArray[mutexNumber-1]);           //unlock the correct mutex
				printf("mutex %d unlocked in periodic task %d\n", mutexNumber, thisTaskID);
            }
        }

		printf("next %lu\n", next.tv_nsec);
		next.tv_nsec = next.tv_nsec + period.tv_nsec; 
		printf("next + period %lu\n", next.tv_nsec);
		if(next.tv_nsec >= 1000000000L){		//if there is overflow...
			next.tv_nsec -= 1000000000L;       	// handle "carry" to seconds field
    		next.tv_sec++;						//increase seconds
		}
        
        
            printf("Task %d should only cancel from now begins\n",thisTaskID);
        
        cancelStateResponse = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        if (cancelStateResponse != 0)
        {
            printf("setting cancel state return non-zero: %d", cancelStateResponse);
        }
        int ns = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
		printf("in task %d this is what nanosleep returns: %d\n",thisTaskID, ns);		//determine if sleep is necessary, if it is, sleep
        cancelStateResponse = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        if (cancelStateResponse != 0)
        {
            printf("setting cancel state return non-zero: %d", cancelStateResponse);
        }
        
            printf("Task %d should only cancel before now (ends)\n",thisTaskID);
        
        
        if(flagToTerminate == 1){
            break;
        }
        clock_gettime(CLOCK_MONOTONIC, &next);
		printf("next after sleep %lu\n", next.tv_nsec);

        i++;	//next task
    }

    
    //printf("periodic thread #%d made it to exit\n", runningTasksCount);

    return NULL;

}

void* aperiodicTask(void * aperiodicTask){
    int cancelStateResponse;
    cancelStateResponse = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (cancelStateResponse != 0)
    {
        printf("setting cancel state return non-zero: %d", cancelStateResponse);
    }

    printf("aperiodic task\n");

	//barrier synchronization
    pthread_mutex_lock(&ready_mut);
    readyTasksCount++;
    pthread_cond_wait(&cond, &ready_mut);
    pthread_mutex_unlock(&ready_mut);

    Task * newTask = (Task *)aperiodicTask;

    int eventID = newTask->event - 1;	//get the event number in terms of the keyboard calue

	//barrier synchronization for event
    pthread_mutex_lock(&event_mut[eventID]);
    readyTasksEvent[eventID]++;
    pthread_cond_wait(&condEvent[eventID], &event_mut[eventID]);
    pthread_mutex_unlock(&event_mut[eventID]);
    
	//run the task
    int i = 0;
    while(i < newTask->bodyLength){			
        if(atoi(newTask->body[i])){		//if the next task in the body is an interation
            compute(atoi(newTask->body[i]));	//run the iteration
            printf("computed %d iterations\n", atoi(newTask->body[i]));
        }
        else{
            char mutexChar;
            int mutexNumber;	
            if(newTask->body[i][0] == 'L'){		//lock mutex
                mutexChar = newTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
                pthread_mutex_lock(&mutexArray[mutexNumber-1]);		//lock the correct mutex
                printf("mutex %d locked\n", mutexNumber);
                
            }
            else if(newTask->body[i][0] == 'U'){	//unlock mutex
                mutexChar = newTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
                pthread_mutex_unlock(&mutexArray[mutexNumber-1]);         //unlock the correct mutex
				printf("mutex %d unlocked\n", mutexNumber);
            }
        }

        i++;		//next task
    }

    //printf("aperiodic thread made it to exit\n");
    //pthread_exit(NULL);


    return NULL;

}



int main(int argc, const char * argv[]) {
	
    FILE *filePointer;	//file pointer that reads from given file
	
	printf("start");
    flagToTerminate = 0;
/*
    char filename[200];
	if(argc > 0){
		strcpy(filename, argv[1]);
	}    
	else{
		printf("Please enter a filename.\n");
		return -1;
	}
*/
    filePointer = fopen("test2.txt", "r");
    if(filePointer == NULL){
        printf("couldn't open file");
        return -1;
    }

    Task ** taskList;
	int i;

    taskList = getInput(filePointer);

	fclose(filePointer);

	printf("tasks have been stored in an array\n");
	printf(" total time %d\n", totalTaskTime);
    if(taskList != NULL){

		printf("set up total time");
		//set up the main period
		struct timespec period;
		period.tv_nsec = (long)totalTaskTime*1000000;
		period.tv_sec = 0;
		int getSeconds = (long)totalTaskTime*1000000/1000000000;
		printf("%d\n", getSeconds);		
		printf("%lu\n", period.tv_nsec);

		i = 0;
		while(i < getSeconds){		//deal with overflow
			if(period.tv_nsec >= 1000000000L){		//if there is overflow...
				period.tv_nsec -= 1000000000L;       	// handle "carry" to seconds field
				period.tv_sec++;						//increase seconds
			}
			i++;
			printf("i %d\n", i);
			printf("%lu\n", period.tv_sec);
		}
		printf("period seconds %lu\n", period.tv_sec);
		printf("period nano seconds %lu\n", period.tv_nsec);

		/*
			pthread_mutex_t my_mutex;
			pthread_mutexattr_t my_mutex_attr;
			pthread_mutexattr_init(&my_mutex_attr);
			pthread_mutexattr_setprotocol(&my_mutex_attr, PTHREAD_PRIO_INHERIT);
			pthread_mutex_init(&my_mutex, &my_mutex_attr);
			pthread_mutexattr_destroy(&my_mutex_attr);
		*/
		
		pthread_t threadID[numTasks];	//initialize the thread id array

		//set mutex array to be pi mutex
		pthread_mutexattr_t piMutexAttr;	
		pthread_mutexattr_init(&piMutexAttr);
		pthread_mutexattr_setprotocol(&piMutexAttr, PTHREAD_PRIO_INHERIT);

		int mutexCount;
		int mutexMax = 10;
		for(mutexCount = 0; mutexCount < mutexMax; mutexCount++){
			pthread_mutex_init(&mutexArray[mutexCount], &piMutexAttr);		//init mutex and set protocol to priority inheritance
		}

		//trying to set main priority to 99
        //setpriority(PRIO_PROCESS, 0, 99);
        
		//set up keyboard listen thread
		struct sched_param kbdParam;		
        kbdParam.sched_priority = 98;
		pthread_t id;
		pthread_create(&id, NULL, &keyboardListen, (void *)NULL);
		pthread_setschedparam(id, SCHED_FIFO, &kbdParam);


		//initialize pthread parameter
    	struct sched_param pthreadParam;
		//create mutiple threads from task list array
		for(i = 0; i < numTasks; i++){
            pthreadParam.sched_priority = taskList[i]->priority;	//set the thread priority
            if(taskList[i]->type == 'P'){	//if the thread is a pthread - create thread with periodictask fucntion
				pthread_create(&threadID[i], NULL, &periodicTask, (void *)taskList[i]); 	//create thread
				pthread_setschedparam(threadID[i], SCHED_FIFO, &pthreadParam);		//set schedule parameter to FIFO
				printf("PERIODIC THREAD %d CREATED\n", i);
				printf("Thread id: %lu\n", (long unsigned)&threadID[i]);
			}
			else if(taskList[i]->type == 'A'){	//if the thread is a pthread - create thread with aperiodictask fucntion
                numTasksEvent[taskList[i]->event - 1]++;		//increment correct event in event array
				pthread_create(&threadID[i], NULL, &aperiodicTask, (void *)taskList[i]);	//create thread
				pthread_setschedparam(threadID[i], SCHED_FIFO, &pthreadParam);		//set schedule parameter to FIFO
				printf("APERIODIC THREAD %d CREATED\n", i);
			}
			
		}
		printf("waiting for tasks to be ready..");

		//wait until all tasks have been created
        while(readyTasksCount != numTasks);		//busy loop until all threads are ready

		//broadcast that all threads have been created
        pthread_cond_broadcast(&cond);		//broadcast signal to start when busy loop exits

		
        //sleep until period ends so terminating signal is sent at the right time
        if(period.tv_sec > 0){
            period.tv_sec--;
            nanosleep(&period, NULL);
        }
        
        //-------- ---------- ---------- signaling threads to terminate ------- -------- -------- begins
        printf("TERMINATE ALL THREADS SIGNAL SENT\n");
        flagToTerminate = 1;
        
        int c = 0;
        for(c = 0 ; c < numTasks; c++){
            pthread_cancel(threadID[c]);
        }
        //-------- ---------- ---------- signaling threads to terminate ------- -------- -------- ends
        
        //busy loop until all running threads terminate
        //while(runningTasksCount != 0);
        
        //aaaand we're done here
        printf("THE END\n");
        
        
        
        
        //anything below this will be deleted later and is only here for now for copy/paste purposes
        
        
        /*
        
        
        
        
        struct sigaction newAction, oldAction;
        newAction.sa_handler = catcher;
        sigemptyset(&newAction.sa_mask);
        
        
        
        sigaction(SIGALRM, NULL, &oldAction);
        if(oldAction.sa_handler != SIG_IGN)
            sigaction(SIGALRM, &newAction, NULL);
        sigaction(SIGTERM, NULL, &oldAction);
        
        //for setittimer
        struct itimerval periodForTimer;
        periodForTimer.it_value.tv_sec = period.tv_sec;
        periodForTimer.it_value.tv_usec = period.tv_nsec * 1000000;
        
        if (setitimer(ITIMER_REAL, &periodForTimer, NULL) == -1) {
            perror("error calling setitimer()");
            exit(1);
        }
        
        nanosleep(&period, NULL);		//determine if sleep is necessary, if it is, sleep
		//THIS IS WHERE WE NEED TO SIGNAL

		//printf("after busy loop and broadcast\n");
        //temporary sleep stuff until we figure out how to stop all tasks
        //struct timespec forsleep;
        //forsleep.tv_sec = 5;
        //forsleep.tv_nsec = 500;
        //nanosleep(&forsleep, NULL);

		//running = 1
		//broadcast signal to end
        
        
         */

    }
    else{
        printf("There are no tasks to run.\n");
    }
    
    
    return 0;
}

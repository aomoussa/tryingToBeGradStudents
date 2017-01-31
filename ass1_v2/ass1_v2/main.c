//  main.c
//  ass1_v2
//
//  Created by Kendall Hickie on 1/20/17.
//  Copyright © 2017 Kendall Hickie. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct{
    char type;
    int priority;
    int event;
    int period;
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
            taskArray->body = malloc((i-1)*sizeof(char*));   //should be
            int j = 0;
            for(j = 0; j < i-1; j++){
                taskArray->body[j] = malloc(sizeof(tempBody[j]));
                strcpy(taskArray->body[j], (const char *)tempBody[j]);
            }
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
        printf("%d\n", i);
        
        if(i > 0){
            taskArray->body = malloc((i-1)*sizeof(char*));   //should be
            int j = 0;
            for(j = 0; j < i-1; j++){
                taskArray->body[j] = malloc(sizeof(tempBody[j]));
                strcpy(taskArray->body[j], (const char *)tempBody[j]);
            }
        }
        
    }
    
    return taskArray;
}


Task ** getInputFromFile(FILE* filePointer){
    
    
    //read the first line
    //determine how many tasks there will be
    //determine the entire time for all tasks
    //read in for the number of lines required (aka the number of taks)
    //store their information inside of the array of structs
    
    char * input;
    size_t numOfBytes = 0;
    int numTasks = 0;
    int totalTaskTime = 0;
    Task ** taskArray;
    
    if(getline(&input, &numOfBytes, filePointer)){
        printf("The input was correct!.\n");
        
        numTasks = atoi(strtok(input," ")); //get the number of tasks
        totalTaskTime = atoi(strtok (NULL, " "));   //get the total task time
        printf("numTasks - %d\n", numTasks);
        printf("taskTime - %d\n", totalTaskTime);
        
        taskArray = (Task **)malloc(numTasks*sizeof(Task *));
        
        //while there is still input, get the next line(s)
        int taskLineCount = 0;
        while(taskLineCount < numTasks){
            if(getline(&input, &numOfBytes, filePointer)){
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

void compute(int x){
    
    int i, j=0;
    for (i = 0; i < x; i++)
    {
        j = j + i;
    }
    printf("compute\n");
}

void periodicTask(Task * periodicTask){
    
    //INIT
    //WAIT FOR ACTIVATION
    printf("Periodic Task\n");
    printf("%c\n", periodicTask->type);
    int i = 0;
    while(periodicTask->body[i] != NULL){
        if(atoi(periodicTask->body[i])){
            compute(atoi(periodicTask->body[i]));
            printf("%d\n", atoi(periodicTask->body[i]));
        }
        else{
            char mutexChar;
            int mutexNumber;
            if(periodicTask->body[i][0] == 'L'){
                mutexChar = periodicTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
                printf("%d\n", mutexNumber);
                //Lock the correct mutex
                //LockMutex(mutexArray[mutexNumber])
                
            }
            else if(periodicTask->body[i][0] == 'U'){
                mutexChar = periodicTask->body[i][1];
                mutexNumber = atoi(&mutexChar);
                printf("%d\n", mutexNumber);
                //unlock the correct mutex
                //UnlockMutex(mutexArray[mutexNumber])
            }
        }
        i++;
        //WAIT FOR PERIOD
        //something with nano_sleep
    }
    //not sure what wait for activation is
    
    
    /*
     < local variables >
     initialization() and wait_for_activation();
     while (condition) {
     <task body>
     wait_for_period();
     }
     */
}

void aperiodicTask(Task * aperiodicTask){
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
    //fclose(filePointer);
    printf("Hello, World! hey Ahmed is here\n");
    Task ** taskList;
    
    
    taskList = getInputFromFile(filePointer);
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
        while(taskList[0]->body[i] != NULL){
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
        while(taskList[1]->body[j] != NULL){
            printf("\t\t%s\n", taskList[1]->body[j]);
            j++;
        }
        
        printf("PeriodIC Task\n");
        periodicTask(taskList[0]);
        
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
    
    
    //while(size of tasks)
    //join threads
    
    
    return 0;
}

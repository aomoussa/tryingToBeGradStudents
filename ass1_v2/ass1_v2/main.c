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
    
    printf ("Splitting line \"%s\" into elements:\n",line);
    
    element = strtok(line," ");
    taskArray->type = *element;
    
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


Task ** getInput(){
    
    //read the first line
    //determine how many tasks there will be
    //determine the entire time for all tasks
    //read in for the number of lines required (aka the number of taks)
    //store their information inside of the array of structs

    char* input;
    size_t numOfBytes = 1;
    int numTasks = 0;
    int totalTaskTime = 0;
    Task ** taskArray;
    
    
    if(getline(&input, &numOfBytes, stdin)){
        printf("The input was correct.\n");
        
        numTasks = atoi(strtok(input," ")); //get the number of tasks
        totalTaskTime = atoi(strtok (NULL, " "));   //get the total task time
        
        int i;
        for(i = 0; i < numTasks; i++){
            taskArray[i] = malloc(sizeof(Task));
        }
        
        //while there is still input, get the next line(s)
        int taskLineCount = 0;
        while(taskLineCount < numTasks){
            if(getline(&input, &numOfBytes, stdin)){
                printf("The input was correct.\n");
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
void periodicTask(Task ** periodicTask){
    {
        < local variables >
        initialization() and wait_for_activation();
        while (condition) {
            <task body>
            wait_for_period();
        }
    }

}

void aperiodicTask(Task ** aperiodicTask){
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

void compute(int x){
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
    
    printf("Hello, World! hey Ahmed is here\n");
    Task ** taskList;
    
    taskList = getInput();
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
        

        
    }
    else{
        printf("someone messed up\n");
    }
    
    //need to run tasks based on p or a in their own thread.... with all 10 mutexes
    
    
    return 0;
}

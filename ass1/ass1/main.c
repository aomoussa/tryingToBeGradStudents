//
//  main.c
//  ass1
//
//  Created by Ahmed Moussa on 1/17/17.
//  Copyright © 2017 Ahmed Moussa. All rights reserved.
//

#include <stdio.h>
#include <string.h>



int parseLine(char* line){
    char* everything[10];
    char* element;
    printf ("Splitting line \"%s\" into elements:\n",line);
    element = strtok(line," ");
    everything[0] = element;
    int i = 1;
    while (element != NULL && i<10)
    {
        printf ("%s\n",element);
        element = strtok (NULL, " ");
        everything[i] = element;
        i++;
    }
    for(int j = i-1; j>=0; j--){
        printf ("element number %d is %s\n", j, everything[j]);
    }
    return 0;
};

int getInput(){
    char* input;                                                    //initiating string for input to be placed in
    int numOfBytes = 1;                                             //number of bytes for input string IDUHTW!!!
    if(getline(&input, &numOfBytes, stdin)){                         //Try the getline, see if it works
        printf("arright, the getline went through\n");
        printf("here's the line: %s",input);
        parseLine(input);
        return 1;
    }
    else{
        printf("getline didn't go through apparently :/\n");
    }
    return 0;
};
int main(int argc, const char * argv[]) {
    
    printf("Hello, World! hey Ahmed is here\n");
    
    
    if(getInput()){                                                 //Try the getInput, see if it works
        printf("back from getInput, it worked!\n");
    }
    else{
        printf("someone messed up\n");
    }
    
    return 0;
}

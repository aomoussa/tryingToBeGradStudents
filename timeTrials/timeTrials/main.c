#include <stdio.h>
#include <time.h>


int compute(int x){
    printf("in compute");
    int i, j=0;
    for (i = 0; i < x; i++)
    {
        j = j + i;
    }
    return 0;
}
int main()
{
    clock_na
    //clock_nan
    int period = 1000;
    int periodInNS = period*1000000;
    struct timespec tim, tim2, diff;
    //tim.tv_sec = 0;
    //tim.tv_nsec = period*1000000; //period is in ms -> ns
    
    printf("yo what the hell is going on?");
    
    int x = 0;
    while(1){
        printf("while loop iteration %d", x++);
        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &tim);
        printf("this is tim %ld", tim.tv_nsec);
        
        compute(200);
        
        clock_gettime(clk_id, &tim2);//get the time and subtract from time 1
        
        if(tim2.tv_nsec < tim.tv_nsec + periodInNS){
            diff.tv_nsec = (tim.tv_nsec + periodInNS) - tim2.tv_nsec;
            if(nanosleep(&diff , NULL) < 0 ) //sleep for subtracted time (period - time1 - time2)
            {
                printf("Nano sleep system call failed \n");
                return -1;
            }
        }
    }
    printf("Nano sleep successfull \n");
    
    return 0;
}

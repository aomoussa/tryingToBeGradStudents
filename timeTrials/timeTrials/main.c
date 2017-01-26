//
//  main.c
//  timeTrials
//
//  Created by Ahmed Moussa on 1/25/17.
//  Copyright © 2017 Ahmed Moussa. All rights reserved.
//

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

static void ms2ts(struct timespec *ts, unsigned long ms)
{
    ts->tv_sec = ms / 1000;
    ts->tv_nsec = (ms % 1000) * 1000000;
}

static void print_ts(unsigned long ms)
{
    struct timespec ts;
    ms2ts(&ts, ms);
    printf("%lu milliseconds is %ld seconds and %ld nanoseconds.\n",
           ms, ts.tv_sec, ts.tv_nsec);
}

int main()
{
    print_ts(1000);
    print_ts(2500);
    print_ts(4321);
    struct timespec time;
    struct timeval t;
    gettimeofday(&t, NULL);
    
    time.tv_sec = t.tv_sec;
    
    
    printf("%lu\n", time.tv_sec);
    
    
    
    
    
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmbuf[64], buf[64];
    
    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
    snprintf(buf, sizeof buf, "%s.%06d", tmbuf, tv.tv_usec);
    
    printf("\n%s", tmbuf);
    
    return 0;
}

#include "tokenBucket.h"
#include <bits/time.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include<time.h>
#include<stdio.h>

//This code gives current Time in nanoseconds
static uint64_t current_time_ns(){
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
};

void token_bucket_init(TokenBucket *bucket, uint64_t rate, uint64_t burst_size){
    bucket->time_ns = 0;
    bucket->time_per_token_ns=1000000000ULL  /rate;
    bucket->time_per_burst_ns = burst_size* bucket->time_per_token_ns;
    pthread_mutex_init(&bucket->mutex , NULL);
};

bool token_bucket_consume(TokenBucket* bucket , uint64_t tokens){
    pthread_mutex_lock(&bucket->mutex);
    uint64_t now = current_time_ns();
    uint64_t time_needed = tokens* bucket->time_per_token_ns;
    uint64_t min_time = now - bucket->time_per_burst_ns;
    uint64_t new_time = bucket->time_ns;
   // here max capacity is Filled  
        if(new_time <min_time){
            new_time = min_time;
        };
        new_time += time_needed;
        //Not enough TOkens
        if(new_time > now){
            pthread_mutex_unlock(&bucket->mutex);
            return false;
        };

        bucket->time_ns = new_time;
        pthread_mutex_unlock(&bucket->mutex);

        return true;
};
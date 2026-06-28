#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <stdint.h>
#include<stdbool.h>
#include<pthread.h>

typedef struct{
    uint64_t time_ns;
    uint64_t time_per_token_ns;
    uint64_t time_per_burst_ns;
    pthread_mutex_t mutex;
} TokenBucket;

//Rate ->> it's measure of tokens generated pre second 
// burst_size -->>> maximum number of tokens stored in bucket(Capacity basically)

void token_bucket_init(TokenBucket* bucket , uint64_t rate , uint64_t burst_size);

//TRUE -> request allowed
//FALSE -> request rejected 
bool token_bucket_consume(TokenBucket* bucket, uint64_t tokens);

#endif
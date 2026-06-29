#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <netinet/in.h>
#include <stdint.h>
#include<stdbool.h>
#include<pthread.h>

typedef struct{
    //time_ns ->> earliest virtual time at which the next request is allowed
    uint64_t time_ns;
    
    //if rate is 5 tokens/sec , then time_per_token_ns is 1/5 sec.
    uint64_t time_per_token_ns;
    
    //if Burst time is 10 , and each token needs 1/5 sec , so 10*1/5 == 2 seconds
    // means bucket can hold 2 seconds worth of requests
    uint64_t time_per_burst_ns;
    //we use mutex , just so that different threads don't try to consume same token or
    // have race conditions 
    pthread_mutex_t mutex;
} TokenBucket;


typedef struct ClientBucket{
    char ip[INET_ADDRSTRLEN];
    TokenBucket bucket;
    struct ClientBucket* next;
} ClientBucket;

typedef struct{
    ClientBucket *head;
    pthread_mutex_t mutex;
    uint64_t rate;
    uint64_t burst;
} RateLimiter;

//it means , if time_ns < current_time , then (current_time - time_ns) per sec worth fo tokens are available
// 
//Rate ->> it's measure of tokens generated pre second 
// burst_size -->>> maximum number of tokens stored in bucket(Capacity basically)

void token_bucket_init(TokenBucket* bucket , uint64_t rate , uint64_t burst_size);

//TRUE -> request allowed
//FALSE -> request rejected 
bool token_bucket_consume(TokenBucket* bucket, uint64_t tokens);
bool allow_request(RateLimiter* rl , const char* ip);
void rate_limiter_init(RateLimiter* rl , uint64_t rate , uint64_t burst);
void rate_limiter_destroy(RateLimiter * rl);
#endif
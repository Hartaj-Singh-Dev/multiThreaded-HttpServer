#include "tokenBucket.h"
#include <bits/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include<time.h>
#include<stdio.h>
#include "string.h"
#include<stdlib.h>
//This code gives current Time in nanoseconds
// tv_sec = seconds
// tv_nsec = nanoseconds
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
      //Spend Tokens-->>  
      new_time += time_needed; 
        //Not enough TOkens
        if(new_time > now){
            pthread_mutex_unlock(&bucket->mutex);
            return false;
        };

     //Equivalent to removing tokens 
       bucket->time_ns = new_time;
        pthread_mutex_unlock(&bucket->mutex);

        return true;
};

static ClientBucket* find_client(RateLimiter* rl , const char* ip){
    ClientBucket * curr = rl->head;
    while(curr != NULL){;
        if(strcmp(curr->ip , ip) == 0){
            return curr;
        };
        curr = curr->next;
    };

    return NULL;
};

static ClientBucket* create_client(RateLimiter* rl , const char* ip){
    ClientBucket * client = malloc(sizeof(ClientBucket));
    if(client == NULL){
        return NULL;
    }
    memset(client , 0 , sizeof(ClientBucket));
    strncpy(client->ip,ip,INET_ADDRSTRLEN -1);
    client->ip[INET_ADDRSTRLEN - 1] = '\0';
    token_bucket_init(&client->bucket, rl->rate, rl->burst);

    client->next = rl->head;
    rl->head = client;
    return client;
    
};

bool allow_request(RateLimiter* rl , const char* ip){
    pthread_mutex_lock(&rl->mutex);
    ClientBucket* client = find_client(rl, ip);
    if(client == NULL){
        client = create_client(rl , ip);
        if(client == NULL){
            pthread_mutex_unlock(&rl->mutex);
            return false;
        }

        printf("Created bucket for %s\n" , ip);
    };

    pthread_mutex_unlock(&rl->mutex);

    return token_bucket_consume(&client->bucket, 1);
};

void rate_limiter_init(RateLimiter *rl, uint64_t rate, uint64_t burst){
    rl->head = NULL;
    rl->rate = rate;
    rl->burst = burst;
    if(pthread_mutex_init(&rl->mutex, NULL) != 0){
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }
    printf("Rate Limiter Initalized\n");
    
};

void rate_limiter_destroy(RateLimiter* rl){
    ClientBucket * curr = rl->head;
    while(curr != NULL){
        ClientBucket * next = curr->next;
        pthread_mutex_destroy(&curr->bucket.mutex);
        free(curr);
        curr = next;
    };

    pthread_mutex_destroy(&rl->mutex);
    printf("rate limiter destroyed \n");
}
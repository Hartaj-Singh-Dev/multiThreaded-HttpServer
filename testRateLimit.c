#include <stdio.h>
#include <unistd.h>
#include "tokenBucket.h"

int main(){
    TokenBucket bucket;
    //5 req/sec or 5 tokens per second
    // 10 is maximum capcacity
    token_bucket_init(&bucket, 5, 10);

    for(int i = 1 ;i <= 20 ;i++){
        if(token_bucket_consume(&bucket , 1)){
            printf("Request: %2d: ALLOWED\n" , i);
        }else{
            printf("Request %2d: REJECTED\n" , i);
        }
        usleep(100000);
    };

    return 0;
}
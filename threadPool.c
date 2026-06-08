// #include <bits/pthreadtypes.h>
// #include <bits/pthreadtypes.h>
#include "threadPool.h"
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define THREAD_COUNT 2

pthread_t workers[THREAD_COUNT];

typedef struct Task
{
    int client_fd;
    struct Task* next;
} Task;

Task* front = NULL;
Task* rear = NULL;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

extern void handle_client(int client_fd);

void* worker_thread(void* arg){
   printf("worker started\n"); 
    while(1){
        pthread_mutex_lock(&queue_mutex);
        while(front==NULL){
            pthread_cond_wait(&queue_cond , &queue_mutex);
        };

        Task * task = front;
        front = front->next;
        if(front == NULL){
            rear = NULL;
        };
        pthread_mutex_unlock(&queue_mutex);
        int client_fd = task->client_fd;
        free(task);
        
        printf("worker %lu handling fd=%d\n" , (unsigned long) pthread_self() , client_fd);
        handle_client(client_fd);
    };

    return NULL;
}


void init_thread_pool(int num_threads){
   printf("init_thread_pool called\n"); 
    for(int i =0 ;i < num_threads ;i++){
        int * worker_id = malloc(sizeof(int));
        if(worker_id == NULL){
            perror("Malloc");
            exit(EXIT_FAILURE);
        };

        *worker_id = i;
        if(pthread_create(&workers[i], NULL, worker_thread, worker_id) != 0){
            perror("Pthread_create");
            exit(EXIT_FAILURE);
        };
    };
};

void add_task(int client_fd){
   printf("task added fd=%d\n", client_fd); 
    Task* task = malloc(sizeof(Task));
    if(task == NULL){perror("Malloc") ; return ;}
    task->client_fd = client_fd;
    task->next = NULL;
    pthread_mutex_lock(&queue_mutex);
    if(rear == NULL){
        front = rear = task;
    }else{
        rear->next = task;
        rear = task;
    };
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&queue_cond);
};

void destroy_thread_pool(){
    for(int i = 0;i < THREAD_COUNT ;i++){
        pthread_cancel(workers[i]);
        pthread_join(workers[i] , NULL);
    };
};
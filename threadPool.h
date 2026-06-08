#ifndef THREADPOOL_H
#define THREADPOOL_H

void init_thread_pool(int num_threads);
void add_task(int client_fd);
void destroy_thread_pool();
#endif
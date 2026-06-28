#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include<stddef.h>

typedef struct CacheNode{
    char* path;
    char * content;
    size_t content_size;
    struct CacheNode* prev;
    struct CacheNode* next;
} CacheNode;

typedef struct{
    int capacity;
    int current_size;
    CacheNode* head;
    CacheNode* tail;
} lruCache;

lruCache* cache_init(int capacity);
CacheNode* cache_get(lruCache* cache , const char *path);

void cache_put(lruCache* chache , const char* path , const char* content , size_t content_size);
void cache_destroy(lruCache* cache);

#endif
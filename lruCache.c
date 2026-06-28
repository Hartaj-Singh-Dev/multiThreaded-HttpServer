#include"lruCache.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static CacheNode* create_node(const char*path , const char* content , size_t size){
    CacheNode* node = malloc(sizeof(CacheNode));
    node->path = strdup(path) ;
    node->content = malloc(size);
    memcpy(node->content, content , size);
    node->content_size = size;
    node->prev = NULL;
    node->next = NULL;

    return node;
};

lruCache* cache_init(int capacity){
    lruCache* cache = malloc(sizeof(lruCache));
    cache->capacity = capacity;
    cache->current_size = 0;

    cache->head = NULL;
    cache->tail = NULL;
    return cache;
}

static void move_to_front(lruCache* cache , CacheNode *node){
    if(node == cache->head){
        return;
    }
    if(node->prev)node->prev->next = node->next;
    if(node->next)node->next->prev = node->prev;
    if(node== cache->tail)cache->tail = node->prev;
    node->prev = NULL;
    node->next = cache->head;

    if(cache->head)cache->head->prev = node;
    cache->head = node;
    if(cache->tail == NULL) cache->tail = node;
    
}
CacheNode* cache_get( lruCache *cache,const char *path){
    CacheNode *curr = cache->head;
    while(curr)
    {
        if(strcmp(curr->path, path) == 0)
        {
            move_to_front(
                cache,
                curr
            );

            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}


static void evict_lru(lruCache *cache){
    if(cache->tail == NULL)    return;

    CacheNode *victim = cache->tail;
    if(victim->prev) {
        victim->prev->next = NULL;
    }

    cache->tail = victim->prev;

    if(cache->tail == NULL) {
        cache->head = NULL;
    }

    free(victim->path);
    free(victim->content);
    free(victim);

    cache->current_size--;
};

void cache_put(lruCache *cache,const char *path, const char *content, size_t size){
    CacheNode *existing =cache_get(cache, path);
    if(existing) return;
    if(cache->current_size >= cache->capacity){
        evict_lru(cache);
    }
    CacheNode *node = create_node(path,content,size);
    node->next = cache->head;
    if(cache->head){
        cache->head->prev = node;
    }
    cache->head = node;
    if(cache->tail == NULL){
        cache->tail = node;
    }
    cache->current_size++;
};

void cache_destroy(lruCache *cache){
    CacheNode *curr =cache->head;
    while(curr){
        CacheNode *next =curr->next;
        free(curr->path);
        free(curr->content);
        free(curr);
        curr = next;
    }

    free(cache);
}

void cache_print(lruCache *cache){
    CacheNode *curr =cache->head;
    printf("\nCache State:\n");
    while(curr){
        printf( "%s -> ", curr->path);
        curr = curr->next;
    }
    printf("NULL\n");
}
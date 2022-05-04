#define _CRT_SECURE_NO_WARNINGS

#include "LRU.h"

/*创建LRU缓存*/
LRUCache* LRUCacheCreate(int capacity)
{
    LRUCache* NewCache = (LRUCache*)malloc(sizeof(LRUCache));
    memset(NewCache, 0, sizeof(*NewCache));
    NewCache->capacity = capacity;
    NewCache->HashMap = malloc(capacity * sizeof(CacheNode*));
    memset(NewCache->HashMap, 0, sizeof(CacheNode*) * capacity);

    return NewCache;
}

/*销毁LRUCache*/
void LRUCacheDestroy(LRUCache* lruCache)
{
    free(lruCache->HashMap);
    freeList(lruCache->LRUHead);
    free(lruCache);
}

/*根据key获得val*/
void LRUCacheGet(LRUCache* lruCache, char* key, time_t* ttl, uint32_t* ip)
{
    CacheNode* Entry = lruCache->HashMap[HashCode(lruCache, key)];

    while (Entry)
    {
        if (!strncmp(Entry->key, key, KEY_SIZE))
            break;
        else
            Entry = Entry->LRUNext;
    }

    if (Entry == NULL)
    {
        ttl = NULL;
        ip = NULL;
    }
    else
    {
        *ttl = Entry->expireTime;
        *ip = Entry->ip;
    }
}

/*将键值对放入cache*/
void LRUCachePut(LRUCache* lruCache, char* key,time_t* ttl, uint32_t* ip)
{
    CacheNode* Entry = lruCache->HashMap[HashCode(lruCache, key)];

    while (Entry)
    {
        if (!strncmp(Entry->key, key, KEY_SIZE))
            break;
        else
            Entry = Entry->LRUNext;
    }

    if (Entry != NULL)
    {
        Entry->expireTime = *ttl;
        Entry->ip = *ip;

        MoveToFirst(lruCache, Entry);
    }
    else
    {
        Entry = NewCacheNode(key, *ttl,*ip);
        if (lruCache->currentSize == lruCache->capacity)
        {
            CacheNode* temp = lruCache->LRUTail->LRUPrev;
            lruCache->LRUTail->LRUPrev = temp->LRUPrev;
            temp->LRUPrev->LRUNext = lruCache->LRUTail;
            free(temp);
            lruCache->currentSize--;
        }
        MoveToFirst(lruCache, Entry);
        HashMapInsert(lruCache, Entry);
        lruCache->currentSize++;
    }
}

/*头插*/
void moveToFirst(LRUCache* cache, cacheNode* entry)
{
    if (cache->LRUHead == NULL && cache->LRUTail == NULL)
    {
        cache->LRUHead = entry;
        cache->LRUTail = entry;
    }
    else 
    {
        entry->LRUNext = cache->LRUHead;
        entry->LRUPrev = NULL;
        cache->LRUHead->LRUPrev = entry;
        cache->LRUHead = entry;
    }
}

cacheNode* newCacheNode(char* key, char* val)
{
    CacheNode* node = (CacheNode*)malloc(sizeof(*node));
    memset(node, 0, sizeof(*node));
    strncpy(node->key, key, KEY_SIZE);
    node->expireTime = ttl;
    node->ip = ip;
    return node;
}

void LRUCachePrint(LRUCache* lruCache)
{
    if (NULL == lruCache || 0 == lruCache->currentSize) {
        return;
    }
    fprintf(stdout, "cache (key data):\n");
    CacheNode* entry = lruCache->LRUHead;
    for (int i = 0; i < lruCache->currentSize;i++) {
        fprintf(stdout, "(%s, %s, %s)\n", entry->key, entry->expireTime,entry->ip);
        entry = entry->LRUNext;
    }
}

int HashCode(LRUCache* cache, char* key)
{
    unsigned int len = strlen(key);
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
    unsigned int i = 0;
    for (i = 0; i < len; key++, i++)
    {
        hash = hash * a + (unsigned int)(*key);
        a = a * b;
    }
    return hash % (cache->capacity);
}

void hashMapInsert(LRUCache* cache, cacheNode* node)
{
    int index = HashCode(cache, node->key);
    CacheNode* n = cache->HashMap[index];

    if (n != NULL)
    {
        node->HashNext = n;
        n->HashPrev = node;
    }
    cache->HashMap[index] = node;
}

void freeList(CacheNode* head)
{
    while (head)
    {
        CacheNode* temp = head->LRUNext;
        free(head);
        head = temp;
    }
}

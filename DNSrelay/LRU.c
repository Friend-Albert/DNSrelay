#define _CRT_SECURE_NO_WARNINGS

#include "LRU.h"

/*创建LRU缓存*/
LRUCache* LRUCacheCreate(int capacity)
{
    LRUCache* newCache = (LRUCache*)malloc(sizeof(LRUCache));
    memset(newCache, 0, sizeof(LRUCache));
    newCache->capacity = capacity;
    for (int i = 0; i < capacity; i++)
        newCache->hashMap[i] = NULL;
    return newCache;
}

/*销毁LRUCache*/
void LRUCacheDestroy(LRUCache* lruCache)
{
    free(lruCache->hashMap);
    freeList(lruCache->LRUHead);
    free(lruCache);
}

/*根据key获得val*/
int LRUCacheGet(LRUCache* lruCache, char* key, time_t* ttl, uint32_t* ip)
{
    cacheNode* entry = lruCache->hashMap[hashCode(lruCache, key)];
    while (entry)
    {
        if (!strncmp(entry->key, key, KEY_SIZE))
            break;
        else
            entry = entry->hashNext;
    }
    if (entry == NULL)
    {
        return 0;
    }
    else
    {
        *ttl = entry->expireTime;
        *ip = entry->ip;
        moveToFirst(lruCache, entry);
        return 1;
    }
}

/*将键值对放入cache*/
void LRUCachePut(LRUCache* lruCache, char* key,time_t ttl, uint32_t ip)
{
    int idx = hashCode(lruCache, key);
    cacheNode* entry = lruCache->hashMap[idx];

    while (entry)
    {
        if (!strncmp(entry->key, key, KEY_SIZE))
            break;
        else
            entry = entry->hashNext;
    }

    if (entry != NULL)
    {
        entry->expireTime = ttl + time(NULL);
        entry->ip = ip;
        if (entry == lruCache->LRUTail)
            lruCache->LRUTail = entry->LRUPrev;
        if(entry != lruCache->LRUHead)
            moveToFirst(lruCache, entry);
    }
    else
    {
        entry = newCacheNode(key, ttl, ip);
        if (lruCache->currentSize == lruCache->capacity)
        {
            cacheNode* temp = lruCache->LRUTail;
            int index = hashCode(lruCache, temp->key);
            lruCache->LRUTail = temp->LRUPrev;
            lruCache->LRUTail->LRUNext = NULL;
            cacheNode* prev = temp->hashPrev;
            if (temp->hashPrev)
                temp->hashPrev->hashNext = temp->hashNext;
            if (temp->hashNext)
                temp->hashNext->hashPrev = prev;
            if (lruCache->hashMap[index] == temp)
                lruCache->hashMap[index] = NULL;
            free(temp);
            lruCache->currentSize--;
        }
        moveToFirst(lruCache, entry);
        hashMapInsert(lruCache, entry);
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
        cacheNode* head = cache->LRUHead;
        if (entry->LRUPrev)
            entry->LRUPrev->LRUNext = entry->LRUNext;
        if (entry->LRUNext)
            entry->LRUNext->LRUPrev = entry->LRUPrev;
        entry->LRUNext = head;
        entry->LRUPrev = NULL;
        head->LRUPrev = entry;
        cache->LRUHead = entry;
    }
}


cacheNode* newCacheNode(char* key, time_t ttl, uint32_t ip)
{
    cacheNode* node = malloc(sizeof(cacheNode));
    node->hashNext = NULL;
    node->hashPrev = NULL;
    node->LRUNext = NULL;
    node->LRUPrev = NULL;
    strncpy(node->key, key, KEY_SIZE);
    node->expireTime = ttl + time(NULL);
    node->ip = ip;
    return node;
}

void LRUCachePrint(LRUCache* lruCache)
{
    if (NULL == lruCache || 0 == lruCache->currentSize) {
        return;
    }
    printf("cache (key data):\n");
    cacheNode* entry = lruCache->LRUTail;
    int i;
    for (i = 0; entry!=NULL;i++) {
        printf("(%s, %d, %x)\n", entry->key, entry->expireTime,entry->ip);
        entry = entry->LRUPrev;
    }
    printf("total:%d\n", i);
}

int hashCode(LRUCache* cache, char* key)
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
    return hash % SIZE;
}

void hashMapInsert(LRUCache* cache, cacheNode* node)
{
    int index = hashCode(cache, node->key);
    cacheNode* n = cache->hashMap[index];

    if (n != NULL)
    {
        node->hashNext = n;
        n->hashPrev = node;
    }
    cache->hashMap[index] = node;
}

void freeList(cacheNode* head)
{
    while (head)
    {
        cacheNode* temp = head->LRUNext;
        free(head);
        head = temp;
    }
}

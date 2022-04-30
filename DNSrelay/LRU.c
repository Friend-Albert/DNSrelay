#define _CRT_SECURE_NO_WARNINGS

#include "LRU.h"

/*创建LRU缓存*/
LRUCache* LRUCacheCreate(int capacity)
{
    LRUCache* newCache = (LRUCache*)malloc(sizeof(LRUCache));
    memset(newCache, 0, sizeof(*newCache));
    newCache->capacity = capacity;
    newCache->hashMap = malloc(capacity * sizeof(cacheNode*));
    memset(newCache->hashMap, 0, sizeof(cacheNode*) * capacity);

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
char* LRUCacheGet(LRUCache* cache, char* key)
{
    cacheNode* Entry = cache->cashMap[hashCode(cache, key)];

    while (Entry)
    {
        if (!strncmp(Entry->key, key, KEY_SIZE))
            break;
        else
            Entry = Entry->LRUNext;
    }

    if (Entry == NULL) return NULL;
    else return Entry->val;
}

/*将键值对放入cache*/
void LRUCachePut(LRUCache* lruCache, char* key, char* val)
{
    cacheNode* Entry = lruCache->hashMap[hashCode(lruCache, key)];

    while (Entry)
    {
        if (!strncmp(Entry->key, key, KEY_SIZE))
            break;
        else
            Entry = Entry->LRUNext;
    }

    if (Entry != NULL)
    {
        strncpy(Entry->val, val, VAL_SIZE);

        MoveToFirst(lruCache, Entry);
    }
    else
    {
        Entry = newCacheNode(key, val);
        if (lruCache->currentSize == lruCache->capacity)
        {
            cacheNode* temp = lruCache->LRUTail->LRUPrev;
            lruCache->LRUTail->LRUPrev = temp->LRUPrev;
            temp->LRUPrev->LRUNext = lruCache->LRUTail;
            free(temp);
            lruCache->currentSize--;
        }
        moveToFirst(lruCache, Entry);
        hashMapInsert(lruCache, Entry);
        lruCache->currentSize++;
    }
}

/*头插*/
void moveToFirst(LRUCache* cache, CacheNode* entry)
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

CacheNode* newCacheNode(char* key, char* val)
{
    cacheNode* node = (cacheNode*)malloc(sizeof(*node));
    memset(node, 0, sizeof(*node));
    strncpy(node->key, key, KEY_SIZE);
    strncpy(node->val, val, VAL_SIZE);
    return node;
}

void LRUCachePrint(LRUCache* lruCache)
{
    if (NULL == lruCache || 0 == lruCache->currentSize) {
        return;
    }
    fprintf(stdout, "cache (key data):\n");
    cacheNode* entry = lruCache->LRUHead;
    for (int i = 0; i < lruCache->currentSize;i++) {
        fprintf(stdout, "(%s, %s)\n", entry->key, entry->val);
        entry = entry->LRUNext;
    }
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
    return hash % (cache->capacity);
}

void hashMapInsert(LRUCache* cache, CacheNode* node)
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

#define _CRT_SECURE_NO_WARNINGS

#include "LRU.h"

/**
 * @brief 创建LRU缓存
 * 
 */
LRUCache* LRUCacheCreate(int capacity)
{
    LRUCache* newCache = (LRUCache*)malloc(sizeof(LRUCache));
    memset(newCache, 0, sizeof(LRUCache));
    newCache->capacity = capacity;
    for (int i = 0; i < capacity; i++)
        newCache->hashMap[i] = NULL;
    return newCache;
}

/**
 * @brief 销毁LRUCache
 * 
 */
void LRUCacheDestroy(LRUCache* lruCache)
{
    free(lruCache->hashMap);
    freeList(lruCache->LRUHead);
    free(lruCache);
}

/**
 * @brief 根据域名查询记录
 * 
 * @param lruCache LRU链表的指针
 * @param key 域名
 * @param ttl 存储ttl查询结果的指针
 * @param ip 存储ip查询结果的指针
 * @return int 0表示没查到
 *             1表示查到
 */
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

/**
 * @brief 将资源记录放入lru链表中
 * 
 * @param lruCache LRU链表的指针
 * @param key 域名
 * @param ttl ttl
 * @param ip ip
 */
void LRUCachePut(LRUCache* lruCache, char* key,time_t ttl, uint32_t ip)
{
    //获得哈希表索引
    int idx = hashCode(lruCache, key);
    cacheNode* entry = lruCache->hashMap[idx];

    //哈希表中查询
    while (entry)
    {
        if (!strncmp(entry->key, key, KEY_SIZE))
            break;
        else
            entry = entry->hashNext;
    }

    //如果查到就更新记录并移至头部
    if (entry != NULL)
    {
        entry->expireTime = ttl + time(NULL);
        entry->ip = ip;
        if (entry == lruCache->LRUTail)
            lruCache->LRUTail = entry->LRUPrev;
        if (entry != lruCache->LRUHead)
            moveToFirst(lruCache, entry);
    }
    //没查到就创建新节点
    else
    {
        entry = newCacheNode(key, ttl, ip);
        //如果满了则释放最久没有使用的记录
        if (lruCache->currentSize == lruCache->capacity)
        {
            cacheNode* temp = lruCache->LRUTail;
            int index = hashCode(lruCache, temp->key);
            //调整lru链表的连接关系
            lruCache->LRUTail = temp->LRUPrev;
            lruCache->LRUTail->LRUNext = NULL;
            //调整所在哈希表的连接关系
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

/**
 * @brief 头插
 * 
 * @param cache LRU链表的指针
 * @param entry 节点指针
 */
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

/**
 * @brief 创建新节点
 * 
 * @param key 域名
 * @param ttl ttl
 * @param ip ip
 * @return cacheNode* 新节点指针
 */
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

/**
 * @brief 打印LRU链表中的所有记录
 * 
 * @param lruCache 
 */
void LRUCachePrint(LRUCache* lruCache)
{
    if (NULL == lruCache || 0 == lruCache->currentSize) {
        return;
    }
    printf("cache (key data):\n");
    cacheNode* entry = lruCache->LRUTail;
    int i;
    for (i = 0; entry!=NULL;i++) {
        printf("(%s, %d, %x)\n", entry->key, (int)entry->expireTime,entry->ip);
        entry = entry->LRUPrev;
    }
}

/**
 * @brief 根据域名计算节点的哈希表索引
 * 
 * @param key 域名
 * @return int 哈希表索引
 */
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

/**
 * @brief 将节点插入哈希表中
 * 
 * @param cache LRU指针
 * @param node 带插入节点
 */
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

/**
 * @brief 释放全部lru空间
 * 
 * @param head LRU链表头结点
 */
void freeList(cacheNode* head)
{
    while (head)
    {
        cacheNode* temp = head->LRUNext;
        free(head);
        head = temp;
    }
}

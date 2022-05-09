#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define KEY_SIZE 254
#define SIZE 100

/*一个LRU缓存，采用链队法哈希表使得查询和插入速度近似为O(1)*/
typedef struct LRUCache{
	int capacity;
	int currentSize;

	struct cacheNode* hashMap[SIZE];

	struct cacheNode* LRUHead;
	struct cacheNode* LRUTail;
}LRUCache;

/*LRU缓存双向链表结点*/
typedef struct cacheNode
{
	char key[KEY_SIZE];
	time_t expireTime;
	uint32_t ip;

	struct cacheNode* hashPrev;
	struct cacheNode* hashNext;

	struct cacheNode* LRUPrev;
	struct cacheNode* LRUNext;
}cacheNode;

LRUCache* LRUCacheCreate(int capacity);
void LRUCacheDestroy(LRUCache* lruCache);
int LRUCacheGet(LRUCache* lruCache, char* key, time_t* ttl, uint32_t* ip);
void LRUCachePut(LRUCache* lruCache,char* key, time_t ttl, uint32_t ip);
cacheNode* newCacheNode(char* key, time_t ttl,uint32_t ip);
void moveToFirst(LRUCache* cache, cacheNode* entry);
void LRUCachePrint(LRUCache* lruCache);
int hashCode(LRUCache* cache, char* key);
void hashMapInsert(LRUCache* cache, cacheNode* node);
void freeList(cacheNode* head);
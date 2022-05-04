#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define KEY_SIZE 50
#define VAL_SIZE 100

/*一个LRU缓存*/
typedef struct LRUCache{
	int capacity;
	int currentSize;

	struct CacheNode** HashMap;

	struct CacheNode* LRUHead;
	struct CacheNode* LRUTail;
}LRUCache;

/*LRU缓存双向链表结点*/
typedef struct CacheNode
{
	char key[KEY_SIZE];
	time_t expireTime;
	//val更改为：
	//time_t expireTime(expireTime应为time(NULL)+ttl) 即系统时间加上报文中的保存记录时长
	uint32_t ip;

	struct CacheNode* HashPrev;
	struct CacheNode* HashNext;

	struct CacheNode* LRUPrev;
	struct CacheNode* LRUNext;
}CacheNode;

/*创建LRU缓存lruCache*/
LRUCache* LRUCacheCreate(int capacity);
/*销毁LRU缓存lruCache*/
void LRUCacheDestroy(LRUCache* lruCache);
/*从LRU缓存中拿出数据*/
void LRUCacheGet(LRUCache* lruCache, char* key, time_t* ttl, uint32_t* ip);
/*将数据放入LRU缓存*/
void LRUCachePut(LRUCache* lruCache,char* key, time_t ttl, uint32_t ip);

/*头插*/
void MoveToFirst(LRUCache* cache, CacheNode* entry);

/*创建cache结点*/
CacheNode* NewCacheNode(char* key, time_t ttl,uint32_t ip);

void LRUCachePrint(LRUCache* lruCache);

int HashCode(LRUCache* cache, char* key);

void HashMapInsert(LRUCache* cache, CacheNode* node);

void freeList(CacheNode* head);
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define KEY_SIZE 50
#define VAL_SIZE 100

/*一个LRU缓存*/
typedef struct LRUCache{
	int capacity;
	int currentSize;

	struct cacheNode** hashMap;

	struct cacheNode* LRUHead;
	struct cacheNode* LRUTail;
}LRUCache;

/*LRU缓存双向链表结点*/
typedef struct cacheNode
{
	char key[KEY_SIZE];
	char val[VAL_SIZE];
	//val更改为：
	//time_t expireTime(expireTime应为time(NULL)+ttl) 即系统时间加上报文中的保存记录时长
	//uint32_t ip

	struct cacheNode* hashPrev;
	struct cacheNode* hashNext;

	struct cacheNode* LRUPrev;
	struct cacheNode* LRUNext;
}cacheNode;

/*创建LRU缓存lruCache*/
LRUCache* LRUCacheCreate(int capacity);
/*销毁LRU缓存lruCache*/
void LRUCacheDestroy(LRUCache* lruCache);
/*从LRU缓存中拿出数据*/
char* LRUCacheGet(LRUCache* lruCache, char* key);//更改为 void LRUCacheGet(LRUCache* lruCache, char* key, time_t* ttl, uint32_t* ip);
/*将数据放入LRU缓存*/
void LRUCachePut(LRUCache* lruCache, char* key, char* val);//更改为 void LRUCachePut(LRUCache* lruCache, time_t* ttl, uint32_t* ip)

/*头插*/
void moveToFirst(LRUCache* cache, cacheNode* entry);

/*创建cache结点*/
cacheNode* newCacheNode(char* key, char* val);

void LRUCachePrint(LRUCache* lruCache);

int hashCode(LRUCache* cache, char* key);

void hashMapInsert(LRUCache* cache, cacheNode* node);

void freeList(cacheNode* head);
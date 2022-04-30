#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
char* LRUCacheGet(LRUCache* lruCache, char* key);
/*将数据放入LRU缓存*/
void LRUCachePut(LRUCache* lruCache, char* key, char* val);

/*头插*/
void moveToFirst(LRUCache* cache, CacheNode* entry);

/*创建cache结点*/
CacheNode* newCacheNode(char* key, char* val);

void LRUCachePrint(LRUCache* lruCache);

int hashCode(LRUCache* cache, char* key);

void hashMapInsert(LRUCache* cache, CacheNode* node);

void freeList(CacheNode* head);
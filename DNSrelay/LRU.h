#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_SIZE 50
#define VAL_SIZE 100

/*һ��LRU����*/
typedef struct LRUCache{
	int capacity;
	int currentSize;

	struct cacheNode** hashMap;

	struct cacheNode* LRUHead;
	struct cacheNode* LRUTail;
}LRUCache;

/*LRU����˫��������*/
typedef struct cacheNode
{
	char key[KEY_SIZE];
	char val[VAL_SIZE];

	struct cacheNode* hashPrev;
	struct cacheNode* hashNext;

	struct cacheNode* LRUPrev;
	struct cacheNode* LRUNext;
}cacheNode;

/*����LRU����lruCache*/
LRUCache* LRUCacheCreate(int capacity);
/*����LRU����lruCache*/
void LRUCacheDestroy(LRUCache* lruCache);
/*��LRU�������ó�����*/
char* LRUCacheGet(LRUCache* lruCache, char* key);
/*�����ݷ���LRU����*/
void LRUCachePut(LRUCache* lruCache, char* key, char* val);

/*ͷ��*/
void moveToFirst(LRUCache* cache, CacheNode* entry);

/*����cache���*/
CacheNode* newCacheNode(char* key, char* val);

void LRUCachePrint(LRUCache* lruCache);

int hashCode(LRUCache* cache, char* key);

void hashMapInsert(LRUCache* cache, CacheNode* node);

void freeList(CacheNode* head);
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

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
	//val����Ϊ��
	//time_t expireTime(expireTimeӦΪtime(NULL)+ttl) ��ϵͳʱ����ϱ����еı����¼ʱ��
	//uint32_t ip

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
char* LRUCacheGet(LRUCache* lruCache, char* key);//����Ϊ void LRUCacheGet(LRUCache* lruCache, char* key, time_t* ttl, uint32_t* ip);
/*�����ݷ���LRU����*/
void LRUCachePut(LRUCache* lruCache, char* key, char* val);//����Ϊ void LRUCachePut(LRUCache* lruCache, time_t* ttl, uint32_t* ip)

/*ͷ��*/
void moveToFirst(LRUCache* cache, cacheNode* entry);

/*����cache���*/
cacheNode* newCacheNode(char* key, char* val);

void LRUCachePrint(LRUCache* lruCache);

int hashCode(LRUCache* cache, char* key);

void hashMapInsert(LRUCache* cache, cacheNode* node);

void freeList(cacheNode* head);
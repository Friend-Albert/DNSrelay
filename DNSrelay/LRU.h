#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define KEY_SIZE 50
#define VAL_SIZE 100

/*һ��LRU����*/
typedef struct LRUCache{
	int capacity;
	int currentSize;

	struct CacheNode** HashMap;

	struct CacheNode* LRUHead;
	struct CacheNode* LRUTail;
}LRUCache;

/*LRU����˫��������*/
typedef struct CacheNode
{
	char key[KEY_SIZE];
	time_t expireTime;
	//val����Ϊ��
	//time_t expireTime(expireTimeӦΪtime(NULL)+ttl) ��ϵͳʱ����ϱ����еı����¼ʱ��
	uint32_t ip;

	struct CacheNode* HashPrev;
	struct CacheNode* HashNext;

	struct CacheNode* LRUPrev;
	struct CacheNode* LRUNext;
}CacheNode;

/*����LRU����lruCache*/
LRUCache* LRUCacheCreate(int capacity);
/*����LRU����lruCache*/
void LRUCacheDestroy(LRUCache* lruCache);
/*��LRU�������ó�����*/
void LRUCacheGet(LRUCache* lruCache, char* key, time_t* ttl, uint32_t* ip);
/*�����ݷ���LRU����*/
void LRUCachePut(LRUCache* lruCache,char* key, time_t ttl, uint32_t ip);

/*ͷ��*/
void MoveToFirst(LRUCache* cache, CacheNode* entry);

/*����cache���*/
CacheNode* NewCacheNode(char* key, time_t ttl,uint32_t ip);

void LRUCachePrint(LRUCache* lruCache);

int HashCode(LRUCache* cache, char* key);

void HashMapInsert(LRUCache* cache, CacheNode* node);

void freeList(CacheNode* head);
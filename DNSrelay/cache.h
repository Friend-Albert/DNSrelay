#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Trie.h"
#include "LRU.h"

#define CACHE_SIZE 100

Trie* tree;
LRUCache* lruCache;

void dotNameToQName(char*);
void qNameToDotName(char*);
void initCache(char*);
uint32_t charIPToIntIP(char*);
void addRecordToCache(char* name, uint32_t ip, time_t ttl);
int getRecordByName(char* name, uint32_t* ip, time_t* ttl);

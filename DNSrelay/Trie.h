#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 253+1
#define CHAR_TYPE 26+10+2
#define MAX_RECORD 3000

//记录字典树信息的结构体
typedef struct{
	int recordNumber;
	uint32_t IP[MAX_RECORD];
	struct node* root;
}Trie;

//字典树节点
typedef struct{
	int index;
	struct node* children[CHAR_TYPE];
}node;

Trie* initTrie();
node* createNode();
int getCharIndex(char);
void insertName(node*, char*, int);
int findIPIndex(node*, char*);
uint32_t getIP(Trie*, char*);
bool deleteName(node*, char*);
void freeTrie(node*);

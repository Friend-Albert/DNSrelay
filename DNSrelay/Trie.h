#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 253+1
#define CHAR_TYPE 26+10+2
#define MAX_RECORD 2000


typedef struct{
	int recordNumber;
	char IP[MAX_RECORD][16];
	struct node* root;
}Trie;

typedef struct{
	int index;
	struct node* children[CHAR_TYPE];
}node;

Trie* initTrie();
node* createNode();
int getCharIndex(char);
void insertName(node*, char*, int);
int findIPIndex(node*, char*);
char* getIP(Trie*, char*);
bool deleteName(node*, char*);
void freeTrie(node*);
bool getRecord(char*, Trie*);

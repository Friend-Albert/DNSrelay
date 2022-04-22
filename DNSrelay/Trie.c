#include "Trie.h"

Trie* initTrie() 
{
	Trie* tree = malloc(sizeof(Trie));
	tree->recordNumber = 0;
	tree->root = createNode();
	return tree;
}

node* createNode()
{
	node* newNode = malloc(sizeof(node));
	memset(newNode->children, 0, sizeof(newNode->children));
	newNode->index = -1;
	return newNode;
}

int getCharIndex(char c)
{
	int index = -1;
	if (c == '-')
		index = 36;
	else if (c == '.')
		index = 37;
	else if (c >= '0' && c <= '9')
		index = 25 + c - '0';
	else
		index = c - 'a';
	return index;
}

void insertName(node* root, char* domainName, int index)
{
	domainName = _strlwr(domainName);
	int len = strlen(domainName) - 1;
	for (int i = 0; i < len; i++) 
	{
		int index = getCharIndex(domainName[i]);
		if (root->children[index] == NULL)
		{
			root->children[index] = createNode();
		}
		root = root->children[index];
	}
	root->index = index;
}

int findIPIndex(node* root, char* domainName)
{
	domainName = _strlwr(domainName);
	int len = strlen(domainName) - 1;
	int index = -1;
	for (int i = 0; i < len; i++)
	{
		int index = getCharIndex(domainName[i]);
		if (root->children[index] == NULL)
		{
			break;
		}
		root = root->children[index];
	}
	index = root->index;
	return index;
}

char* getIP(Trie* trie, char* domainName)
{
	int index = findIPIndex(trie->root, domainName);
	if (index == -1) return NULL;
	char* IP = malloc(sizeof(char)*16);
	memcpy(IP, trie->IP[index], 16);
	return IP;
}

bool deleteName(node* root, char* domainName)
{
	domainName = _strlwr(domainName);
	int len = strlen(domainName) - 1;
	int index = -1;
	for (int i = 0; i < len; i++)
	{
		int index = getCharIndex(domainName[i]);
		if (root->children[index] == NULL)
		{
			return false;
		}
		root = root->children[index];
	}
	root->index = -1;
	return true;
}

void freeTrie(node* root)
{
	for (int i = 0; i < CHAR_TYPE; i++) 
	{
		if (root->children[i]) 
		{
			freeTrie(root->children[i]);
		}
		free(root);
	}
}

bool getRecord(char* path, Trie* tree)
{
	FILE* fp = fopen(path, "rt");
	if (fp == NULL) 
	{ 
		printf("Failed to open the file!\n");
		return false; 
	}
	char tmp[300];
	while (fgets(tmp, 300, fp) != NULL) 
	{
		//if (tmp[0] == '\n') continue;
		int i = 0, j = 0;
		char IP[16];
		char name[MAX_NAME_LENGTH];
		for (; i < strlen(tmp); i++) 
		{
			if (tmp[i] == ' ') break;
			IP[j++] = tmp[i];
		}
		IP[j] = '\0';
		j = 0;
		++i;
		for (; i < strlen(tmp); i++) 
		{
			name[j++] = tmp[i];
		}
		name[--j] = '\0';
		insertName(tree->root, name, tree->recordNumber);
		memcpy(tree->IP[tree->recordNumber++], IP, 16);
		printf("%s\n", name);
	}
	printf("Get %d records,using %zd bytes\n", tree->recordNumber, sizeof(tree));
	return true;
}

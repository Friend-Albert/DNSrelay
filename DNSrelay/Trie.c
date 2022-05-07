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

uint32_t getIP(Trie* trie, char* domainName)
{
	int index = findIPIndex(trie->root, domainName);
	if (index == -1) return -1;
	return trie->IP[index];
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
#include "Trie.h"

/**
 * @brief 创建字典树
 * 
 * @return Trie* 字典树指针
 */
Trie* initTrie() 
{
	Trie* tree = malloc(sizeof(Trie));
	tree->recordNumber = 0;
	tree->root = createNode();
	return tree;
}

/**
 * @brief 新建字典树节点
 * 
 * @return node* 结点指针
 */
node* createNode()
{
	node* newNode = malloc(sizeof(node));
	memset(newNode->children, 0, sizeof(newNode->children));
	newNode->index = -1;
	return newNode;
}

/**
 * @brief 获得字符对应的children数组下标
 * 
 * @param c 待判定字符
 * @return int children下标
 */
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

/**
 * @brief 插入记录
 * 
 * @param root 字典树树根
 * @param domainName 域名
 * @param index 域名对应IP存储的数组下标
 */
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

/**
 * @brief 查询域名对应IP的数组下标
 * 
 * @param root 字典树树根
 * @param domainName 域名
 * @return int != -1 时：IP下标  
 * 			   == -1 时：未查询到
 */
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

/**
 * @brief findIPIndex的进一步封装，直接获得IP
 * 
 * @param trie 字典树树根
 * @param domainName 域名
 * @return uint32_t int类型的IP
 */
uint32_t getIP(Trie* trie, char* domainName)
{
	int index = findIPIndex(trie->root, domainName);
	if (index == -1) return -1;
	return trie->IP[index];
}

/**
 * @brief 删除域名对应记录
 * 
 * @param root 字典树树根
 * @param domainName 域名
 * @return true 删除成功
 * @return false 删除失败（域名不存在）
 */
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

/**
 * @brief 递归释放字典树空间
 * 
 * @param root 字典树树根
 */
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
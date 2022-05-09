#include "cache.h"

/**
 * @brief 将点分十进制ip转化为qname格式的ip
 * 
 * @param name 点分十进制ip字符串
 */
void dotNameToQName(char* name)
{
    char* siz_byte = name;
    *siz_byte = 0;
    name++;
    while (*name)
    {
        if (*name == '.')
        {
            siz_byte = name;
            *siz_byte = 0;
            name++;
        }
        (*siz_byte)++;
        name++;
    }
}

/**
 * @brief 将qname格式IP转换为点分十进制IP
 * 
 * @param qName qname格式IP
 */
void qNameToDotName(char* qName)
{
	for (int i = 1; i < strlen(qName); i++)
	{
		char c = qName[i];
		if ((c == '-') || (tolower(c) <= 'z' && tolower(c) >= 'a') ||
			(c >= '0' && c <= '9'))
			qName[i - 1] = c;
		else
			qName[i - 1] = '.';
	}
	qName[strlen(qName) - 1] = '\0';
}

/**
 * @brief 初始化缓存模块
 * 
 * @param path 资源文件的路径
 */
void initCache(char* path)
{
	//初始化字典树以及LRU链表
	tree = initTrie();
	lruCache = LRUCacheCreate(CACHE_SIZE);
	
	//读取资源文件中的记录
	FILE* fp = fopen(path, "rt");
	time_t ttl = 86400;
	uint32_t ip = 0;
	if (fp == NULL)
	{
		printf("Failed to open the file!\n");
		return;
	}
	char tmp[400] = {0};
	while (fgets(tmp, 400, fp) != NULL)
	{
		int i = 0, j = 0;
		char IP[16];
		char n[MAX_NAME_LENGTH];
		for (; i < strlen(tmp); i++)
		{
			if (tmp[i] == ' ') break;
			IP[j++] = tmp[i];
		}
		IP[j] = '\0';
		ip = charIPToIntIP(IP);
		j = 0;
		++i;
		for (; i < strlen(tmp); i++)
		{
			n[j++] = tmp[i];
		}
		n[--j] = '\0';
		char* name = _strlwr(n);
		if (tree->recordNumber < MAX_RECORD)
		{
			insertName(tree->root, name, tree->recordNumber);
			tree->IP[tree->recordNumber++] = ip;
		}
		LRUCachePut(lruCache, name, ttl, ip);
	}
	printf("Get %d records\n", tree->recordNumber);
	
}

/**
 * @brief 将点分十进制的IP转换为32位的IP
 * 
 * @param ip 点分十进制的IP字符串
 * @return uint32_t 字符串对应的int类型IP
 */
uint32_t charIPToIntIP(char* ip)
{
	int n = 0,j = 0;
	//IP四个字段转换为int的偏移量
	int offset[4] = {24,16,8,0};
	uint32_t intIP = 0;
	for (int i = 0; i < strlen(ip); i++) 
	{
		if (ip[i] == '.')
		{
			intIP |= (n << offset[j++]);
			n = 0;
		}
		else
		{
			n = n * 10 + ip[i] - '0';
		}
	}
	intIP |= (n << offset[j]);
	return intIP;
}

/**
 * @brief 将新纪录添加入缓存中
 * 
 * @param name 域名
 * @param ip int类型IP
 * @param ttl 资源生成记录数值
 */
void addRecordToCache(char* name, uint32_t ip, time_t ttl)
{
	qNameToDotName(name);
	LRUCachePut(lruCache, name, ttl, ip);
}

/**
 * @brief Get the Record By Name object
 * 
 * @param name 域名
 * @param ip 存储查询IP结果的指针
 * @param ttl 存储查询TTL结果的指针
 * @return int 0代表未找到
 * 			   1代表已找到
 */
int getRecordByName(char* name, uint32_t* ip, time_t* ttl)
{
	qNameToDotName(name);
	int get = 0;
	get = LRUCacheGet(lruCache, name, ttl, ip);
	if (get)
	{
		//LRU资源记录超时
		if (*ttl <= time(NULL))
			get = 0;
		//未超时则更新ttl
		else
			*ttl -= time(NULL);
	}
	if (!get) 
	{
		uint32_t IP = -1;
		IP = getIP(tree,name);
		if (IP == -1)
			get = 0;
		else
		{
			*ip = IP;
			*ttl = 86400;
			get = 1;
		}
	}
	return get;
}

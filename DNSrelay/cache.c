#include "cache.h"

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

void initCache(char* path)
{
	tree = initTrie();
	lruCache = LRUCacheCreate(CACHE_SIZE);
	
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

uint32_t charIPToIntIP(char* ip)
{
	int n = 0,j = 0;
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

void addRecordToCache(char* name, uint32_t ip, time_t ttl)
{
	qNameToDotName(name);
	LRUCachePut(lruCache, name, ttl, ip);
}

int getRecordByName(char* name, uint32_t* ip, time_t* ttl)
{
	qNameToDotName(name);
	int get = 0;
	get = LRUCacheGet(lruCache, name, ttl, ip);
	if (get)
	{
		if (*ttl <= time(NULL))
			get = 0;
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

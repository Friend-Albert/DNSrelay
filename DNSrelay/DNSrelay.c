// DNSrelay.cpp: 定义应用程序的入口点。
//

#include "DNSrelay.h"
#include "Trie.h"

#define DEFAULTPATH "D:/project/DNSrelay/DNSrelay/dnsrelay.txt"

int main()
{
	char* IP;
	char name[300];
	Trie* tree = initTrie();
	getRecord(DEFAULTPATH,tree);
	while (scanf("%s", name) && name[0] != '$') {
		IP = getIP(tree, name);
		if (IP) printf("%s\n", IP);
		else printf("No Result\n");
	}
	return 0;
}

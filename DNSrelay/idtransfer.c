#include "idtransfer.h"

uint32_t addId(struct sockaddr_in* clnt, uint32_t clntId)
{
	if (poolRemain == 0)
	{
		printf("Error: Id pool overflowed. please wait a monment.\n");
		return MAX_IDPOOL_SIZE;
	}
	uint32_t prevId = curId;
	while (idPool[curId].hold && time(NULL) - idPool[curId].inTime < 2)
	{
		++curId;
	}
	--poolRemain;
	idPool[curId].hold = true;
	idPool[curId].clntId = clntId;
	idPool[curId].clntAddr.sin_addr = clnt->sin_addr;
	idPool[curId].clntAddr.sin_family = clnt->sin_family;
	idPool[curId].clntAddr.sin_port = clnt->sin_port;
	idPool[curId].inTime = time(NULL);
	return curId++;
}

void releaseId(uint32_t serverId)
{
	idPool[serverId].hold = false;
	++poolRemain;
	return ;
}

struct sockAddr_in* getClientAddr(uint32_t serverId)
{
	return &idPool[serverId].clntAddr;
}

uint32_t getClientId(uint32_t serverId)
{
	return htons(idPool[serverId].clntId);
}

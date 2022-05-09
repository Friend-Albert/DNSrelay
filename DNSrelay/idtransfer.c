#include "idtransfer.h"

/**
 * @brief 完成客户端->服务器id的转换
 * 
 * @param clnt 记录客户端信息的结构体指针
 * @param clntId 客户端DNS请求包的ID
 * @return uint32_t 发送给服务器的响应包ID
 */
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

/**
 * @brief 将ID池中指定项释放
 * 
 * @param serverId 收到DNS响应包的ID
 */
void releaseId(uint32_t serverId)
{
	idPool[serverId].hold = false;
	++poolRemain;
	return ;
}

/**
 * @brief 获得客户端信息
 * 
 * @param serverId 收到DNS响应包的ID
 * @return struct sockaddr_in* 记录客户端信息的结构体指针
 */
struct sockaddr_in* getClientAddr(uint32_t serverId)
{
	return &idPool[serverId].clntAddr;
}

/**
 * @brief 获得客户端请求包的ID
 * 
 * @param serverId 收到DNS响应包的ID
 * @return uint32_t 客户端请求包的ID
 */
uint32_t getClientId(uint32_t serverId)
{
	return htons(idPool[serverId].clntId);
}

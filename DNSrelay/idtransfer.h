#pragma once

#include<string.h>
#include <stdio.h>
#include <winsock2.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#pragma comment (lib, "ws2_32.lib")

#define MAX_LENGTH 65536

struct idTransfer {
	bool hold;
	uint32_t clntId;
	struct sockaddr_in clntAddr;
	time_t inTime;
};

struct idTransfer idPool[MAX_LENGTH];
static int curId = 0;
static int poolRemain = MAX_LENGTH;

uint32_t addId(struct sockaddr_in* clnt, uint32_t clntId);
void releaseId(uint32_t serverId);
struct sockAddr_in* getClientAddr(uint32_t serverId);
uint32_t getClientId(uint32_t serverId);

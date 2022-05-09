// DNSrelay.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include "getopt.h"
#include "idtransfer.h"
#include "cache.h"
#pragma comment (lib, "ws2_32.lib")

#define IOC_VENDOR 0x18000000
#define _WSAIOW(x,y) (IOC_IN|(x)|(y))
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
#define DNSPORT 53
#define BUFSIZE 512

static char DNSSERVER[16] = "114.114.114.114";

//使用位域技巧构造header结构体从而大幅优化代码量同时大幅减小位运算的使用
struct header
{
    unsigned id : 16;    /* query identification number */
    unsigned rd : 1;     /* recursion desired */
    unsigned tc : 1;     /* truncated message */
    unsigned aa : 1;     /* authoritive answer */
    unsigned opcode : 4; /* purpose of message */
    unsigned qr : 1;     /* response flag */
    unsigned rcode : 4;  /* reply code */
    unsigned cd : 1;     /* checking disabled by resolver */
    unsigned ad : 1;     /* authentic data from named */
    unsigned z : 1;      /* unused bits, must be ZERO */
    unsigned ra : 1;     /* recursion available */
    uint16_t qdcount;    /* number of question entries */
    uint16_t ancount;    /* number of answer entries */
    uint16_t nscount;    /* number of authority entries */
    uint16_t arcount;    /* number of resource entries */
};

// DNS报文question字段的偏移量
static const int QUESTION_OFFSET = 12;
struct question
{
    char qname[256];
    uint16_t qtype;
    uint16_t qclass;
};

struct resourceRecord
{
    uint16_t name;
    uint16_t type;
    uint16_t class;
    uint16_t ttl;
    uint16_t _ttl;//此处为阻止结构体内存对齐因此将ttl拆成两部分
    uint16_t rd_length;
    uint32_t rdata;
};

static char path[256] = "D:\\project\\DNSrelay\\DNSrelay\\dnsrelay.txt";//默认资源记录路径
static uint32_t packageCount = 0;//用于记录收到DNS报文数量
static int debugLevel = 1;//调试信息打印级别
static const int aNameOffset = 0xc00c;//域名偏移量
static const uint16_t aNameType = 0x0001;//A类型
static const uint16_t inClass = 1;//IN（Internet）类型
static const uint16_t aRDLength = 4;//A类型的资源数据记录长度（IP地址长度）
static const int rrSize = 16;//A类型资源记录的长度

SOCKET sock;
struct sockaddr_in clntAddr, servAddr;

void printProgramInfo();
void parsingParameters(int, char*);
void initSocket();
uint8_t* getQuestion(struct question*, uint8_t*);
uint16_t get16Bits(uint8_t**);
inline int isDot(char);
void printPackage(uint8_t*,bool);
void printHeader(struct header*);
void handleServerPackage(uint8_t*);
void handleClientPackage(uint8_t*,int);
// TODO: 在此处引用程序需要的其他标头。

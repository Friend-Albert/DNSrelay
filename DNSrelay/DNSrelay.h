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

// Header
//                                 1  1  1  1  1  1
//   0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                      ID                       |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                    QDCOUNT                    |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                    ANCOUNT                    |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                    NSCOUNT                    |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                    ARCOUNT                    |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
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

// Question Section
static const int QUESTION_OFFSET = 12;
struct question
{
    char qname[256];
    uint16_t qtype;
    uint16_t qclass;
};

//Resource Record Format
struct resourceRecord
{
    uint16_t name;
    uint16_t type;
    uint16_t class;
    uint16_t ttl;
    uint16_t _ttl;
    uint16_t rd_length;
    uint32_t rdata;
};

static char path[256] = "D:\\project\\DNSrelay\\DNSrelay\\dnsrelay.txt";
static uint32_t packageCount = 0;
static int debugLevel = 1;
static const int aNameOffset = 0xc00c;
static const uint16_t aNameType = 0x0001;
static const uint16_t inClass = 1;
static const uint16_t aRDLength = 4;
static const int rrSize = 16;

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

// DNSrelay.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdint.h>
#include <time.h>
#include "idtransfer.h"
#pragma comment (lib, "ws2_32.lib")

#define DNSPORT 53
#define BUFSIZE 512
#define DNSSERVER "192.168.2.1"

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

static uint32_t packageCount = 0;
static int debugLevel = 1;
uint8_t* getQuestion(struct question*, uint8_t*);
uint16_t get16Bits(uint8_t**);
inline bool isDot(char);
void printPackage(uint8_t*,int,bool);
void printHeader(struct header*);
// TODO: 在此处引用程序需要的其他标头。

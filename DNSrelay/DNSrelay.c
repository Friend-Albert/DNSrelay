// DNSrelay.cpp: 定义应用程序的入口点。
//

#include "DNSrelay.h"



int main()
{
    //scanf("%d", &debugLevel);
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //创建套接字
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    const bool REUSE = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &REUSE, sizeof(bool));

    struct sockaddr_in relayAddr;
    memset(&relayAddr, 0, sizeof(relayAddr));  //每个字节都用0填充
    relayAddr.sin_family = PF_INET;  //使用IPv4地址
    relayAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
    relayAddr.sin_port = htons(DNSPORT);  //端口
    bind(sock, (SOCKADDR*)&relayAddr, sizeof(SOCKADDR));

    //接收客户端请求
    SOCKADDR clnt;  //客户端地址信息
    SOCKADDR* clntAddr = &clnt;
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = PF_INET;  //使用IPv4地址
    servAddr.sin_addr.s_addr = inet_addr(DNSSERVER);
    servAddr.sin_port = htons(DNSPORT);  //端口
    
    int nSize = sizeof(SOCKADDR);
    uint8_t buffer[BUFSIZE];  //缓冲区
    int idx = 0;
    while (1) 
    {
        int recvLen = recvfrom(sock, buffer, BUFSIZE, 0, clntAddr, &nSize);
        if (recvLen < 0) break;
        struct header* p = (struct header*)buffer;
        if (p->qr == 0 && ntohs(p->qdcount) == 1) //客户端的请求报文且请求数为1
        {
            printPackage(buffer, recvLen, false);
            uint32_t newId = addId(clntAddr, ntohs(p->id));
            p->id = htons(newId);
            sendto(sock, buffer, recvLen, 0, &servAddr, &nSize);
        }
        else if (p->qr == 0 && ntohs(p->qdcount) != 1)//客户端的请求报文且请求数不为1
        {
            printPackage(buffer, recvLen, false);
            uint32_t newId = addId(clntAddr, ntohs(p->id));
            p->id = htons(newId);
            sendto(sock, buffer, recvLen, 0, &servAddr, &nSize);
        }
        else if (p->qr == 1) //DNS服务器的响应报文
        {
            printPackage(buffer, recvLen, true);
            clntAddr = (SOCKADDR*)getClientAddr(ntohs(p->id));
            releaseId(ntohs(p->id));
            p->id = getClientId(ntohs(p->id));
            sendto(sock, buffer, recvLen, 0, clntAddr, &nSize);
        }
    }
    
    printf("failed %d", WSAGetLastError());
    closesocket(sock);
    WSACleanup();
	return 0;
}

uint8_t* getQuestion(struct question* query, uint8_t* buffer)
{
    int i = 0;
    while (0 != *buffer)
    {
        query->qname[i++] = *buffer;
        buffer++;
    }
    query->qname[i] = 0;
    buffer++;
    query->qtype = get16Bits(&buffer);
    query->qclass = get16Bits(&buffer);
    return buffer;
}

uint16_t get16Bits(uint8_t** bufferPtr)
{
    uint16_t res;
    memcpy(&res, *bufferPtr, 2);
    *bufferPtr += 2;
    return ntohs(res);
}

inline bool isDot(char c)
{
    if (c == '-') return false;
    else if (tolower(c) <= 'z' && tolower(c) >= 'a') return false;
    return true;
}

void printPackage(uint8_t* buffer,int len,bool isServer)
{
    time_t rawTime = time(NULL);
    char* timeStr = ctime(&rawTime);
    timeStr[strlen(timeStr) - 1] = 0;
    printf("#%d %s ", packageCount++, timeStr);
    if(isServer)
        printf("Server ");
    else
        printf("Client ");
    struct header* p = (struct header*)buffer;
    if(debugLevel > 1) 
        printHeader(p);
    buffer += QUESTION_OFFSET;
    for (int i = 0; i < ntohs(p->qdcount); i++) 
    {
        struct question query;
        buffer = getQuestion(&query, buffer);
        int j = 1;
        while(query.qname[j] != 0)
        {
            if (isDot(query.qname[j]))
                printf(".");
            else printf("%c", query.qname[j]);
            ++j;
        }
        printf(" type:%02x class:%02x\n", query.qtype, query.qclass);
    }
    
}

void printHeader(struct header* p)
{
    printf("Header:\n");
    printf("id:%04x\n", ntohs(p->id)); 
    printf("QR : %d Opcode : %x AA : %d TC : %d\n",  p->qr, p->opcode, p->aa,p->tc);
    printf("RD : %d RA: %d Z : %d rcode : %x\n", p->rd, p->ra, p->z, p->rcode);
    printf("qdcount:%d ancount:%d arcount:%d nscount:%d\n", 
        ntohs(p->qdcount), ntohs(p->ancount), ntohs(p->arcount), ntohs(p->nscount));
}

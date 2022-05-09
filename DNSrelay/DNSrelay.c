// DNSrelay.cpp: 定义应用程序的入口点。
//

#include "DNSrelay.h"


int main(int argc, char* argv[])
{   
    printProgramInfo();
    //parsingParameters(argc, argv);
    initCache(path);
    //scanf("%d", &debugLevel);
    initSocket();
    
    int nSize = sizeof(SOCKADDR);
    uint8_t buffer[BUFSIZE];  //缓冲区
    int idx = 0;
    while (1) 
    {
        //不断接受包，随后针对不同DNS包进行处理
        memset(buffer, 0, sizeof(buffer));
        int recvLen = recvfrom(sock, buffer, BUFSIZE, 0, &clntAddr, &nSize);
        if (recvLen <= 0) { continue; }
        
        struct header* p = (struct header*)buffer;
        if (p->qr == 0 && ntohs(p->qdcount) == 1) //客户端的请求报文且请求数为1
        {
            printPackage(buffer, false);
            handleClientPackage(buffer,recvLen);
        }
        else if (p->qr == 0 && ntohs(p->qdcount) != 1)//客户端的请求报文且请求数不为1
        {
            printPackage(buffer, false);
            //直接向外转发
            uint32_t newId = addId(&clntAddr, ntohs(p->id));
            if (newId == MAX_IDPOOL_SIZE) continue;
            p->id = htons(newId);
            sendto(sock, buffer, recvLen, 0, &servAddr, nSize);
        }
        else if (p->qr == 1) //DNS服务器的响应报文
        {
            printPackage(buffer, true);
            //如果查询到了结果并且问题数为1
            if (1 == ntohs(p->qdcount) && 0 == p->opcode) 
                handleServerPackage(buffer);
            struct sockAddr_in* client;
            client = (SOCKADDR*)getClientAddr(ntohs(p->id));
            releaseId(ntohs(p->id));
            p->id = getClientId(ntohs(p->id));
            sendto(sock, buffer, recvLen, 0, client, nSize);
        }
    }
    
    printf("failed %d", WSAGetLastError());
    closesocket(sock);
    WSACleanup();
    system("pause");
	return 0;
}

/**
 * @brief 打印程序基本信息
 * 
 */
void printProgramInfo()
{
    printf("                                 DNSRELAY\n");
    printf("@Auther: He shaoChen Ma JunMeng Yu KeXin\n");
    printf("@Teacher: Cheng Li\n");
    printf("@Description: A simple DNS server, course design of computer network\n");
    printf("@Usage: dnsrelay [-d] [-h] [-l] [-f <db-file>] [-s <dns-server>]\n");
    printf("---------------------------------------------------------------------------\n");
}

/**
 * @brief 处理命令行参数
 * 
 * @param argc 参数数量
 * @param argv 参数字符串数组
 */
void parsingParameters(int argc, char* argv[])
{
    int o;
    const char* optstring = "dhlf:s:";
    while ((o = getopt(argc, argv, optstring)) != -1)
    {
        switch (o)
        {
        case 'd':
            debugLevel = 2;
            printf("DEBUG mode is on.\n");
            break;
        case 'l':
            debugLevel = 1;
            printf("LOG mode is on.\n");
            break;
        case 'f':
            strcpy(path, optarg);
            break;
        case 's':
            strcpy(DNSSERVER, optarg);
            break;
        case 'h':
        default:
            printf("Usage:  dnsrelay [-d] [-h] [-l] [-p <port>] [-f <db-file>] [-s <dns-server>]\n");
            printf("Where:  -d                     (print debug information)\n");
            printf("        -h                     (print help)\n");
            printf("        -l                     (print log)\n");
            printf("        -f db-file             (specify DNS table file)\n");
            printf("        -s dns-server          (specify DNS server)\n");
            exit(-1);
        }
    }
}

/**
 * @brief 初始化socket
 * 
 */
void initSocket()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //创建套接字
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    const int REUSE = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*) & REUSE, sizeof(REUSE));

    memset(&clntAddr, 0, sizeof(clntAddr));  //每个字节都用0填充
    clntAddr.sin_family = PF_INET;  //使用IPv4地址
    clntAddr.sin_addr.s_addr = htonl(INADDR_ANY); //自动获取IP地址
    clntAddr.sin_port = htons(DNSPORT);  
    bind(sock, (SOCKADDR*)&clntAddr, sizeof(SOCKADDR));

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = PF_INET;  //使用IPv4地址
    servAddr.sin_addr.s_addr = inet_addr(DNSSERVER);
    servAddr.sin_port = htons(DNSPORT);  

    //此处为解决windows下10054（连接被远端主机强制关闭）的bug
    DWORD  dwByteReturned = 0;
    BOOL bNewBehavior = FALSE;
    DWORD status;
    status = WSAIoctl(sock, SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior), 
        NULL, 0, &dwByteReturned, NULL, NULL);
}

/**
 * @brief 解析DNS报文中的question字段
 * 
 * @param query 记录question字段信息的结构体的指针
 * @param buffer 接受DNS报文的缓冲区
 * @return uint8_t* 资源记录字段的起始位置
 */
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

/**
 * @brief 获得主机序的16位数据
 * 
 * @param bufferPtr 指向缓冲区某位置的指针
 * @return uint16_t 转换为主机序的16位数据
 */
uint16_t get16Bits(uint8_t** bufferPtr)
{
    uint16_t res;
    memcpy(&res, *bufferPtr, 2);
    *bufferPtr += 2;
    return ntohs(res);
}

/**
 * @brief 判断qname字段的字符是否应转换为"."
 * 
 * @param c 待判断字符
 * @return int 0表示不应转换为.
 *             1表示应转换为.
 */
inline int isDot(char c)
{
    if (c == '-') return 0;
    else if (tolower(c) <= 'z' && tolower(c) >= 'a') return 0;
    else if (c >= '0' && c <= '9') return 0;
    return 1;
}

/**
 * @brief 打印DNS报文信息
 * 
 * @param buffer 接受报文的缓冲区
 * @param isServer 是否为服务器发送来的报文
 */
void printPackage(uint8_t* buffer,bool isServer)
{
    struct header* p = (struct header*)buffer;
    //在debug模式下会打印报头的详细信息
    if(debugLevel > 1) 
        printHeader(p);
    time_t rawTime = time(NULL);
    char* timeStr = ctime(&rawTime);
    timeStr[strlen(timeStr) - 1] = 0;
    printf("#%d %s ", packageCount++, timeStr);
    if(isServer)
        printf("Server ");
    else
        printf("Client ");
    buffer += QUESTION_OFFSET;
    struct question question;
    struct question* q = &question;
    for (int i = 0; i < ntohs(p->qdcount); i++) 
    {
        buffer = getQuestion(q, buffer);
        int j = 1;
        while(q->qname[j] != 0)
        {
            if (isDot(q->qname[j]))
                printf(".");
            else printf("%c", q->qname[j]);
            ++j;
        }
        printf(" type:%02x class:%02x ", q->qtype, q->qclass);
    }
}

/**
 * @brief 打印报头详细信息
 * 
 * @param p 记录报头信息的结构体指针
 */
void printHeader(struct header* p)
{
    printf("---------------------------------------------------------------------\n");
    printf("                             Header:\n");
    printf("id:%04x\n", ntohs(p->id)); 
    printf("QR : %d Opcode : %x AA : %d TC : %d\n",  p->qr, p->opcode, p->aa,p->tc);
    printf("RD : %d RA: %d Z : %d rcode : %x\n", p->rd, p->ra, p->z, p->rcode);
    printf("qdcount:%d ancount:%d arcount:%d nscount:%d\n", 
        ntohs(p->qdcount), ntohs(p->ancount), ntohs(p->arcount), ntohs(p->nscount));
}

/**
 * @brief 处理服务器返回的报文
 * 
 * @param buffer 
 */
void handleServerPackage(uint8_t* buffer)
{
    //获得资源记录起始位置的指针
    buffer += QUESTION_OFFSET;
    struct question question;
    uint8_t* recordOffset = getQuestion(&question,buffer);

    //获得资源记录
    struct resourceRecord* record = (struct resourseRecord*)recordOffset;
    
    //如果答案域名同请求域名一致并且为A类型则添加记录至LRU中
    if (ntohs(record->name) == aNameOffset && ntohs(record->type) == aNameType)
    {
        addRecordToCache(question.qname, ntohl(record->rdata), (time_t)ntohl(*(uint32_t*)&record->ttl));
    }
    printf("\n");
}

/**
 * @brief 处理客户端发来的报文
 * 
 * @param buffer 记录DNS报文的缓冲区
 * @param len DNS报文长度
 */
void handleClientPackage(uint8_t* buffer,int len) 
{
    uint8_t* buf = buffer;
    uint32_t ip = 0;
    time_t ttl = 0;
    int nSize = sizeof(SOCKADDR);
    struct header* p = (struct header*)buffer;
    buffer += QUESTION_OFFSET;
    struct question question;
    uint8_t* recordOffset = getQuestion(&question, buffer);
    //如果找到记录
    if (question.qtype == 1 && getRecordByName(question.qname, &ip, &ttl))
    {
        p->qr = 1;
        p->ra = 1;
        //如果该域名被屏蔽
        if (ip == 0)
            p->rcode = 3;
        //自行构造资源记录字段并返回
        else
        {
            p->rcode = 0;
            p->ancount = htons(1);
            struct resourceRecord answer;
            answer.name = htons(aNameOffset);
            answer.type = htons(aNameType);
            answer.class = htons(inClass);
            *(uint32_t*)&answer.ttl = htonl(ttl);
            answer.rd_length = htons(aRDLength);
            answer.rdata = htonl(ip);
            memcpy(recordOffset, (uint8_t*)(&answer), rrSize);
            len += rrSize;
        }
        printf("(Hit local record! ttl:%lld)\n",ttl);
        sendto(sock, buf, len, 0, (struct sockaddr*)&clntAddr, nSize);
    }
    //如果没有查询到结果则转发至服务器
    else
    {
        printf("\n");
        uint32_t newId = addId(&clntAddr, ntohs(p->id));
        if (newId == MAX_IDPOOL_SIZE) return;
        p->id = htons(newId);
        sendto(sock, buf, len, 0, (struct sockaddr*)&servAddr, nSize);
    }

}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <ctype.h>


#define SERVER_PROT 8089
#define LOCAL_IPADDRESS "172.16.136.101"
#define MAX_LISTEN 128
#define BUFFER_SIZE 128


/* integer bit operation */
#define BITS_MASK(bit)       ((bit) < 64 ? (1LLU << (bit)) : 0LLU)
#define BITS_SET(value, bit) ((value) |= (1LLU << (bit)))
#define BITS_CLR(value, bit) ((value) &= ~(1LLU << (bit)))
#define BITS_TST(value, bit) (!!((value) & (1LLU << (bit))))


int mian()
{
    /* 创建socket套接字 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }


    /* 将本地的IP和端口绑定 */
    struct sockaddr_in localAddress;
    bzero((void *)&localAddress, sizeof(localAddress));
    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(SERVER_PROT);
    inet_pton(AF_INET, LOCAL_IPADDRESS, &(localAddress.sin_addr.s_addr));

    int localAddressLen = sizeof(localAddress);
    int ret = bind(sockfd, (struct sockaddr *)&localAddress, localAddressLen);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }


    /* 监听 */
    ret = listen(socket, MAX_LISTEN);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }

    /* 集合 */
    fd_set readSet;
    /* 清空结合 */
    FD_ZERO(&readSet);
    /* 把监听的文件描述符添加到读集合里面，让内核帮忙检测 */
    FD_SET(sockfd, &readSet);


    int maxfd = sockfd;
#if 0
    /* 设置超时 */
    struct timeval timeValue;
    bzero((void *)&timeValue, sizeof(timeValue));
    timeValue.tv_sec = 5;
    timeValue.tv_usec = 10;

#endif 

    fd_set tempReadSet;
    bzero((void *)&tempReadSet, sizeof(tempReadSet));

    while (1)
    {
        /* 备份读集合 */
        tempReadSet = readSet;

        ret = select(maxfd + 1, &tempReadSet, NULL, NULL, NULL);
        if (ret == -1)
        {
            perror("select error");
            break;
        }

        /* 如果sockfd 在readSet集合里面 */
        if (FD_ISSET(sockfd, &tempReadSet));
        {
            int acceptfd = accept(sockfd, NULL, NULL);
            if (acceptfd == -1)
            {
                perror("accept error");
                break;
            }
            /* 将通信的句柄 放到读集合 */
            FD_SET(acceptfd, &readSet);

            /* 更新maxfd,文件描述符可能空出来 */
            maxfd = maxfd < acceptfd ? acceptfd : maxfd;
        }

        /* 程序到这个地方说明也许有通信 */
        for (int idx = 0; idx <= maxfd; idx++)
        {
            if (idx != sockfd && FD_ISSET(idx, &tempReadSet))
            {
                /* 程序到这里说明一定有通信(已经建立连接过的) */
                char buffer[BUFFER_SIZE];
                bzero((void *)buffer, sizeof(buffer));

                int readBytes = read(idx, buffer, sizeof(buffer) -1);
                if (readBytes < 0)
                {
                    perror("read error");
                    /* 关闭文件句柄 */
                    close(idx);
                    /* 这边还要继续处理其他readBytes */
                    continue;
                }
                else if (readBytes == 0)
                {
                    printf("客户端断开连接...\n");
                    /* 将该通信句柄从读集合中删掉 */
                    FD_CLR(idx, &readSet);
                    /* 关闭通信句柄 */
                    close(idx);
                    continue;
                }
                else
                {
                    printf("recv: %s\n", buffer);

                    for (int jdx = 0; jdx < readBytes; jdx++)
                    {
                        buffer[jdx] = toupper(buffer[jdx]);
                    }

                    /* 发回客户端 */
                    write(idx, buffer, readBytes);
                    usleep(500);
                }
            }
        }

    }
    

    /* 关闭文件描述符 */
    close(socket);





    return 0;
}
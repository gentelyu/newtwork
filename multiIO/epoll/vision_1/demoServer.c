 #include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <ctype.h>

#define SERVER_PORT 8089
#define MAX_LISTEN 128
#define LOCAL_IPADDRESS "172.16.136.101"              
#define BUFFER_SIZE 128


void sigHander(int sigNum)
{
    int ret = 0;

    
}

int main()
{
    /* 信号注册 */
    // signal(SIGINT, sigHander);
    // signal(SIGQUIT, sigHander);
    // signal(SIGTSTP, sigHander);

    /* 
    *   创建socket套接字
    */
    int sockfd = socket(AF_INET, SOCK_STREAM,0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }


    struct sockaddr_in localAddress;
    memset(&localAddress, 0, sizeof(localAddress));
    /* 地址族 */
    localAddress.sin_family = AF_INET;
    /* 端口需要转成大端 */
    localAddress.sin_port = htons(SERVER_PORT);
    /* ip地址需要转成大端 */
    inet_pton(AF_INET, LOCAL_IPADDRESS, &(localAddress.sin_addr.s_addr));

    int localAddressLen = sizeof(localAddress);
    int ret = bind(sockfd, (struct sockaddr *)&localAddress, localAddressLen);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /*
    *   监听
    */
    ret = listen(sockfd, MAX_LISTEN);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }


    /* 创建epoll 红黑树实例*/
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll create error");
        exit(-1);
    }

    /* 将socket添加到红黑树实例里面 */
    struct epoll_event event;
    /* 清除脏数据 */
    memset(&event, 0, sizeof(event));
    event.data.fd = sockfd;
    event.events = EPOLLIN;                     //读事件
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    if (ret == -1)
    {
        perror("epoll_ctl error");
        exit(-1);
    }

    /* 监听的数量 */
    int nums = 0;
    /* 读到的数量 */
    int readBytes = 0;

    while(1)
    {
        struct epoll_event events[BUFFER_SIZE];
        /* 清楚脏数据 */
        memset(events, 0, sizeof(events));

        nums = epoll_wait(epfd, events, BUFFER_SIZE, -1);
        if (nums ==-1)
        {
            perror("epoll_wait error");
            exit(-1);
        }

        /* 程序执行到这个地方有两种情况:
            1.超时
            2.有监听数据来了
         */
        for (int idx = 0; idx < nums; idx++)
        {
            int fd = events[idx].data.fd;
            if (fd == sockfd)
            {
                /* 有连接 */
                int connectfd = accept(sockfd, NULL,NULL);
                if (connectfd == -1)
                {
                    perror("perror error");
                    exit(-1);
                }

                struct epoll_event conn_event;
                memset(&event, 0, sizeof(event));
                conn_event.data.fd = connectfd;
                conn_event.events = EPOLLIN;

                /* 将通信的人添加到红黑树的节点中 */
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connectfd, &conn_event);
                if (ret == -1)
                {
                    perror("epoll_ctl error");
                    continue;
                }    
            }   
            else
            {
                char buffer[BUFFER_SIZE] = { 0 };
                /* 有数据通信 */
                readBytes = read(fd, buffer, sizeof(buffer) - 1);
                if (readBytes < 0)
                {
                    perror("read error");
                    break;
                }
                else if(readBytes == 0)
                {
                    printf("客户端下线.....\n");

                    /* 将该该文件句柄 从红黑树节点中删除 */
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    /* 关闭文件描述符 */
                    close(fd);
                }
                else
                {
                    printf("recv : %s\n", buffer);

                    for (int jdx = 0; jdx < readBytes; jdx++)
                    {
                        buffer[jdx] = toupper(buffer[jdx]);
                    }

                    /* 发回客户端 */
                    write(fd, buffer, sizeof(buffer));

                    sleep(5);
                }
            }
        }
            
    }
    close(sockfd);
    
    return 0;
}
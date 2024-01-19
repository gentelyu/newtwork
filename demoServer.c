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


    /*
    *   绑定
    */
#if 0
    /* 这个结构体不好用 */
    struct sockaddr localAddress;
#else
    struct sockaddr_in localAddress;
    memset(&localAddress, 0, sizeof(localAddress));
    /* 地址族 */
    localAddress.sin_family = AF_INET;
    /* 端口需要转成大端 */
    localAddress.sin_port = htons(SERVER_PORT);
    /* ip地址需要转成大端 */
#if 0
    /* Address to accept any incoming message */
    /* INADDR_ANY = 0x00000000 */
    localAddress.sin_addr.s_addr = INADDR_ANY;
#else 
    inet_pton(AF_INET, LOCAL_IPADDRESS, &(localAddress.sin_addr.s_addr));
#endif

    int localAddressLen = sizeof(localAddress);
#endif
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


    /* 
    *   接收
    */
    socklen_t clientAddressLen = 0;
    socklen_t clientAddress = 0;
    int acceptfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (acceptfd == -1)
    {
        perror("accept error");
        exit(-1);
    }


    /*
    *   读
    */
    char buff[BUFFER_SIZE];
    while (1)
    {
        ret = read(acceptfd, buff,sizeof(buff) - 1);
        if (ret <= 0)
        {
            perror("read error");
            close(acceptfd);
            break;
        }
        else
        {
            printf("%s\n", buff);
            memset(buff, 0, sizeof(buff));

            scanf("%s",buff);
            ret = write(acceptfd,buff,sizeof(buff) -1);
            if (ret < 0)
            {
                perror("write error");
            }
        }
    }

    close(sockfd);


    return 0;
}
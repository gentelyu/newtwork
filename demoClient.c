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
#define SERVER_IP "172.16.136.101"
#define BUFFER_SIZE 128

int main()
{
    /*
    *   创建套接字
    */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }


    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));


    /* 端口 */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);

    int ret = inet_pton(AF_INET, SERVER_IP, (void *)&(serverAddress.sin_addr.s_addr));
    if (ret != 1)
    {
        perror("inet_pton error");
        exit(-1);
    }


    /* ip地址 */
    ret = connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (ret == -1)
    {
        perror("connect error");
        exit(-1);
    }


    /* 
    *   客户发送信息 
    */
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // char *message = "我是俞晨，世界上最强的男人";
    // strcpy(buffer, message);

    while(1)
    {
        scanf("%s", buffer);
        ret = write(sockfd,buffer,sizeof(buffer) -1);
        if (ret < 0)
        {
            perror("write error");
        }
        memset(buffer, 0, sizeof(buffer));
        sleep(5);

        ret = read(sockfd, buffer,sizeof(buffer) - 1);
        if (ret < 0)
        {
            perror("read error");
            break;
        }
        else if (ret == 0)
        {
            //todo...
        }
        else if (ret > 0)
        {
            printf("%s\n", buffer);
        }
    }

    close(sockfd);


    return 0;
}
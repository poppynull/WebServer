#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
int main(){

    //1. 创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("socket");
        exit(-1);
    }

    //2. 连接服务器
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(9999);
    connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    //3.通信
    while(1){
        // const char* data= "hello, I am client!";
        char recvBuf[1024];
        bzero(&recvBuf, sizeof(recvBuf));
        scanf("%s", recvBuf);
        int len_w = write(sockfd, recvBuf, strlen(recvBuf));
        // write(sockfd, data, strlen(data));
        if(len_w < 0){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        bzero(&recvBuf, sizeof(recvBuf));
        int len = read(sockfd, recvBuf, sizeof(recvBuf));
        if(len == 0) {
            printf("clinet closed...");
            break;
        }
        else if(len > 0){
            printf("recv server data : %s\n", recvBuf);
        }
        else{
            close(sockfd);
            perror("read");
            exit(-1);
        }
    }
    close(sockfd);
    return 0;
}
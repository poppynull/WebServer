#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
int main(){

    //1. 创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);


    //2.绑定
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockaddr.sin_port = htons(9999);
    bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));

    //3.监听
    listen(sockfd, 8);

    //4.accept
    struct sockaddr_in clientaddr;
    int len = sizeof(clientaddr);
    int cfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

    // 输出客户端信息
    char clientIP[16] ;
    // 将网络字节序的IP整数转换成点分十进制的IP字符串
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    printf("client ip is %s, port is %d\n", clientIP, clientPort);

    //5. 通信
    
    while(1){
        char revBuf[1024] = {0};
        bzero(revBuf, sizeof(revBuf));
        int len = read(cfd, revBuf, sizeof(revBuf));
        if(len == 0) {
            printf("clinet closed...");
            break; 
        }
        else if(len > 0){
            printf("recv client data :%s\n", revBuf);

            // 给客户端发送数据
            bzero(revBuf, sizeof(revBuf));
            scanf("%s", revBuf);
            int write_bytes = write(cfd, revBuf, sizeof(revBuf));
            // if(write_bytes < 0){
            //     printf("socket already disconnected, can't write any more!\n");
            //     break;
            // }
        }
        else{
            perror("read");
            exit(-1);
        }
        
    }

    close(cfd);
    close(sockfd);

    return 0;
}
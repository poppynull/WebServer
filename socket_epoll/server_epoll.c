#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
int main()
{
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // 监听
    listen(lfd, 8);

    // 调用epoll_create()创建一个epoll实例
    int epfd = epoll_create(100);

    // 将监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

    struct epoll_event epevs[1024];

    while(1){
        int ret = epoll_wait(epfd, epevs, 1024, -1);
        printf("ret: %d\n", ret);
        for(int i = 0; i < ret; i++){
            int curfd = epevs[i].data.fd;
            if(curfd == lfd){ //新的客户端连接
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(curfd, (struct sockaddr *)&cliaddr, &len);  // 客户端连接
                epev.data.fd = cfd;
                epev.events = EPOLLIN;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd, &epev);
            }
            else{ //有数据读入
                char recvBuf[1024] = {0};
                int len = read(curfd, recvBuf, sizeof(recvBuf));
                if(len > 0){
                    printf("read buf = %s\n", recvBuf);
                    write(curfd, recvBuf, sizeof(recvBuf));
                }
                else if(len == 0){
                    printf("client close\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                }
                else{
                    perror("read");
                    exit(-1);
                }
            }
        }
    }

    close(lfd);
    close(epfd);
    return 0;
}
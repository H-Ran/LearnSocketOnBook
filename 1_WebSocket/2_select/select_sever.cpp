#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/time.h>
#include <vector>
#include <errno.h>

// 自定义的无效fd值
#define INVALID_FD -1
const char *SERVER_ADDR = "127.0.0.1";

int main()
{
    // 创建一个监听socket
    auto listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == INVALID_FD)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }
    // 初始化sockaddr
    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    bindaddr.sin_port = htons(10004);
    if (bind(listenfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        std::cout << "bind listen socket error." << std::endl;
        return -1;
    }
    // 启动监听
    if (listen(listenfd, SOMAXCONN))
    {
        std::cout << "listen error." << std::endl;
        return -1;
    }

    /*----------------------------------------------------------------------------------------------*/
    // 存储客户端的socket数组
    std::vector<int> clientfds;
    int maxfd; // 指向最后一个(最大)一个socket

    while (true)
    {
        fd_set readset;
        FD_ZERO(&readset);

        // 将listen's socket加入待检测的可读事件中
        FD_SET(listenfd, &readset);

        maxfd = listenfd;

        // 将客户端fd加入待检测的可读事件中
        for (auto clientfd : clientfds)
        {
            if (clientfd != INVALID_FD)
            {
                FD_SET(clientfd, &readset);
                if (clientfd > maxfd)
                    maxfd = clientfd;
            }
        }

        // 设置超时时间
        timeval tm;
        tm.tv_sec = 1;
        tm.tv_usec = 0;
        /* 1 seconds */
        // 目前只检测可读事件,不检查写和异常事件
        int ret = select(maxfd + 1, &readset, NULL, NULL, &tm);
        if (ret == -1)
        {
            // 出错抛出异常,退出程序
            if (errno != EINTR)
                break;
        }
        else if (ret == 0)
        {
            // select超出timeval时间,下次继续检测
            continue;
        }
        else
        {
            // 检测到某个socket有事件->listenfd
            if (FD_ISSET(listenfd, &readset))
            {
                // listen socket 的可读事件,表示有新连接到来触发
                struct sockaddr_in clientaddr;
                socklen_t clientaddrlen = sizeof(clientaddr);

                // 接受客户端的连接
                int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
                if (clientfd == INVALID_FD)
                {
                    // accept数据出错,退出程序
                    break;
                }
                std::cout << "accept a client connection, fd : " << clientfd << std::endl;
                clientfds.push_back(clientfd);
            }
            // 检测到事件但不是listenfd,则是clientfd,开始接收数据
            else
            {
                // 假设对端发来的数据长度不超过63个字符
                char recvbuf[64];
                for (int i = 0; i < clientfds.size(); i++)
                {
                    auto clientfd = clientfds[i];
                    if (clientfd != INVALID_FD && FD_ISSET(clientfd, &readset))
                    {
                        memset(recvbuf, 0, sizeof(recvbuf));

                        int length = recv(clientfd, recvbuf, sizeof(recvbuf), 0);
                        if (length <= 0)
                        {
                            // 收取数据出错
                            std::cout << "recv data error,clientfd: " << clientfd << std::endl;
                            close(clientfd);
                            clientfds[i] = INVALID_FD;
                            continue;
                        }

                        std::cout << "clientfd: " << clientfd << ", recv data : " << recvbuf << std::endl;
                    }
                }
            }
        }
    }
    // 关闭所有客户端socket
    for (auto clientfd : clientfds)
    {
        if (clientfd != INVALID_FD)
        {
            close(clientfd);
        }
    }

    close(listenfd);
    return 0;
}
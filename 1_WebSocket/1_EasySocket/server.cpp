#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main(int argc, char *argv[])
{
    /// 1.create listener socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }
    /// 2.Initlaize serve address
    // about port,ip_address,ip_family
    struct sockaddr_in bindaddr; // socket 地址

    bindaddr.sin_family = AF_INET; // 地址族
    /*
        常见的地址族有:
    - AF_INET: IPv4地址族,用于Internet协议第4版
    - AF_INET6: IPv6地址族,用于Internet协议第6版
    - AF_UNIX: UNIX域套接字,用于同一台机器上的进程间通信
    - AF_PACKET: 用于 Packet sockets 发送或接收数据包
    - AF_NETLINK: 用于内核用户空间通信的套接字    */

    bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 转换成对应大端小端->0.0.0.0:接收任意本地地址连接

    bindaddr.sin_port = htons(10004);

    if (bind(listenfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        std::cout << "bind listen socket error." << std::endl;
        return -1;
    }

    /// 3. 启动监听
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
        return -1;
    }

    // loop to recvive data
    while (true)
    {
        std::cout << "wait for recving." << std::endl;
        // defined stuct about client's address
        // just blank

        struct sockaddr_in clientaddr;
        socklen_t clientaddr_len = sizeof(clientaddr);

        /// 4. 接收客户端的连接
        int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddr_len);
        // int clientfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
        if (clientfd != -1)
        {
            char recvBuf[64] = {0};

            /// 5. 接收客户端发送的数据
            int ret = recv(clientfd, recvBuf, 32, 0);
            // 如果收到了客户端的数据
            if (ret > 0)
            {
                std::cout << "recv data from client, data: " << recvBuf << std::endl;

                /// 6.将收到的数据再次发送给客户端
                // ret接收发送错误码
                ret = send(clientfd, recvBuf, std::strlen(recvBuf), 0);
                // if return has not equal than recvBuf
                if (ret != std::strlen(recvBuf))
                {
                    std::cout << "send data error." << std::endl;
                }
                else
                {
                    std::cout << "send data to client successfully, data:" << recvBuf << std::endl;
                }
            }
            // 接收不到client's data
            else
            {
                std::cout << "recv data error." << std::endl;
            }

            close(clientfd);
        }
    }

    /// 关闭监听soket
    close(listenfd);

    return 0;
}
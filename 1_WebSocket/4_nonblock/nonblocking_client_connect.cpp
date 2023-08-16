#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <cstring>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 3000
const char *SEND_DATA = "hello I'm clinet";

int main(int argc, char **argv)
{
    // 1.创建socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
    {
        std::cout << "create client socket error." << std::endl;
        return -1;
    }

    // 将fd设置为非阻塞模式
    int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1)
    {
        close(clientfd);
        std::cout << "set socket to nonblock error." << std::endl;
        return -1;
    }

    // 2.连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);

    for (;;)
    {
        int ret = connect(clientfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

        if (ret == 0)
        {
            std::cout << "connect to server successfully." << std::endl;
            close(clientfd);
            return 0;
        }
        else if (ret == -1)
        {
            if (errno == EINTR)
            {
                std::cout << "connecting interruptted by signal, try again." << std::endl;
                continue;
            }
            else if (errno == EINPROGRESS)
            {
                // 尝试连接中
                std::cout << "trying to connecting......" << std::endl;
                // break;
                continue;
            }
            else
            {
                // 出错了
                std::cout << "connect error!" << std::endl;
                close(clientfd);
                return -1;
            }
        }
    }
    fd_set write_set;
    FD_ZERO(&write_set);
    FD_SET(clientfd, &write_set);
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    // 3.调用select函数判断socket是否可写
    if (select(clientfd + 1, NULL, &write_set, NULL, &tv) == 1)
    {
        std::cout << "[select] connect to server successfully." << std::endl;
    }
    else
    {
        std::cout << "[select] connect to server error." << std::endl;
    }

    // 用错误码和getsockopt检测socket是否出错
    int err;
    socklen_t clientfd_len = static_cast<socklen_t>(sizeof(err));
    if (getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &err, &clientfd_len) < 0)
    {
        close(clientfd);
        return -1;
    }
    // getsockopt错误码为0则表示连接上；
    if (err == 0)
    {
        std::cout << "connect to server successfully." << std::endl;
    }
    else
    {
    std::cout << "connect to server error." << std::endl;
    }

    close(clientfd);
    return 0;
}
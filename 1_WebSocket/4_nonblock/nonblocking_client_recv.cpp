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
    }

    //+++将socket绑定到0 port
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(0);
    if (bind(clientfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
    {
        std::cout << "bind listen socket error." << std::endl;
        return -1;
    }

    // 2.连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(clientfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "connect socket error." << std::endl;
        return -1;
    }

    // 连接服务器后，将clientfd设置为非阻塞模式
    // 不能在刚创建是就设置，会影响到connect函数的行为
    // ？TODO:是什么行为？
    int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1)
    {
        close(clientfd);
        std::cout << "set socket to nonblock error." << std::endl;
        return -1;
    }
    // 3.不断向服务器发送数据，或者出错退出
    int count = 0;
    while (true)
    {
        char recvbuf[32] = {0};
        //!:因为clientfd设置为非阻塞模式，无论是否接收到有数据，recv都不会阻塞程序
        int ret = recv(clientfd, recvbuf, 32, 0);
        // 返回了错误
        if (ret == -1)
        {
            // 非阻塞模式下，无数据可读，错误码EWOLUDBLOCK
            if (errno == EWOULDBLOCK)
            {
                std::cout << "There is no data available now." << std::endl;
                continue;
            }
            else if (errno == EINTR)
            {
                // 如果被信号中断干扰到错误码，重试一次
                std::cout << "sending data interrupted by signal." << std::endl;
                continue;
            }
            else
            {
                // 出错了
                std::cout << "recv data error." << std::endl;
                break;
            }
        }
        else if (ret == 0)
        {
            // 对端关闭了连接，我们也关闭
            std::cout << "peer close the socket." << std::endl;
            break;
        }
        else
        {
            count++;
            std::cout << "send data successfully,count = " << count << std::endl;
        }
    }
    // 关闭socket
    close(clientfd);
    return 0;
}
// 阻塞模式下send 和 recv函数的行为
// 不使用recv接收clinet消息
// 溢出后阻塞
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

int main(int argc, char **argv)
{
    // 1.监听socket
    int server_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_listen_fd == -1)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }
    // 2.初始化服务器地址
    struct sockaddr_in bind_server_addr;
    bind_server_addr.sin_family = AF_INET;
    bind_server_addr.sin_port = htons(3000);
    bind_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int bind_success = 0;
    bind_success = bind(server_listen_fd, (struct sockaddr *)&bind_server_addr, sizeof(bind_server_addr));
    if (bind_success)
    {
        std::cout << "bind listen socket error." << std::endl;
        close(server_listen_fd);
        return -1;
    }
    // 3.启动监听
    if (listen(server_listen_fd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
        close(server_listen_fd);
        return -1;
    }
    while (true)
    {
        struct sockaddr_in bind_client_addr;
        socklen_t client_addr_len = sizeof(bind_client_addr);
        // 4.接收客户端连接
        int client_fd = accept(server_listen_fd, (struct sockaddr *)&bind_client_addr, &client_addr_len);
        if (client_fd != -1)
        {
            // 只接受连接，不调用recv收取任何数据
            std::cout << "accept a client connection." << std::endl;
        }
    }
    close(server_listen_fd);
    return 0;
}
// 获取当前
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <vector>

const int INVALID_FD = -1;

int main(int argc, char **argv)
{
    // 1.监听socket
    int server_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_listen_fd == -1)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }

    // 2.将监听socket设置为非阻塞的
    int old_server_listen_flag = fcntl(server_listen_fd, F_GETFL, 0);
    int new_server_listen_flag = old_server_listen_flag | O_NONBLOCK;
    if (fcntl(server_listen_fd, F_SETFL, new_server_listen_flag) == 1)
    {
        close(server_listen_fd);
        std::cout << "set server listen fd error." << std::endl;
        return -1;
    }

    // 复用地址和端口号
    int on = 1;
    setsockopt(server_listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    setsockopt(server_listen_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));

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

    // pollfd集合
    std::vector<pollfd> fds;
    pollfd listen_fd_info;
    listen_fd_info.fd = server_listen_fd;
    listen_fd_info.events = POLLIN;
    listen_fd_info.revents = 0;
    fds.push_back(listen_fd_info);

    // 是否存在无效的fd标志
    bool exist_invaild_fd;
    int n;

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
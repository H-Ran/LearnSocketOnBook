#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 10004
const char *SEND_DATA = "helloworld";

int main(int argc, char **argv)
{
    // 1.创建socket
    int clinetfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clinetfd == -1)
    {
        std::cout << "create client socket error." << std::endl;
    }

    //+++将socket绑定到0 port
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(0);
    if (bind(clinetfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
    {
        std::cout << "bind listen socket error." << std::endl;
        return -1;
    }

    // 2.连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(clinetfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "connect socket error." << std::endl;
        return -1;
    }

    // 3.向服务器发送数据
    send(clinetfd, SEND_DATA, strlen(SEND_DATA), 0);

    // 4.从服务器收取数据
    char recvBuf[32] = {0};
    int ret = recv(clinetfd, recvBuf, 32, 0);
    if (ret > 0)
        std::cout << "recv data successfully,data:" << recvBuf << std::endl;

    // 5.关闭socket
    close(clinetfd);

    while (true)
    {
        sleep(3);
    }
    return 0;
}

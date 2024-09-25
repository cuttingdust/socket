#include "XTCP.h"

#include <cstring>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

class TcpThread
{
public:
    /// 线程函数
    void Main()
    {
        /// 处理客户端数据
        char buffer[BUFFER_SIZE] = {0};

        for (;;)
        {
            const int recvlen = client_xtcp_.recv(buffer, BUFFER_SIZE);
            if (recvlen <= 0)
            {
                printf("Client disconnected or error occurred.\n");
                break;
            }
            buffer[recvlen] = '\0';
            if (strstr(buffer, "quit") != NULL)
            {
                printf("Client quit.\n");
                client_xtcp_.send("quit success\n", 13);
                break;
            }
            printf("Recv: %s\n", buffer);
            client_xtcp_.send("ok\n", 3); /// Echo back
        }

        client_xtcp_.close();
        delete this;
    }

public:
    XTCP client_xtcp_;
};

#ifdef __LINUX__
#include <sys/epoll.h>
#elif __APPLE__
#include <sys/event.h>
#endif


int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "client") == 0)
        {
            printf("Client start.\n");
            XTCP xclient;
            xclient.createSocket();
            // xclient.setBlock(false);

            xclient.connect("127.0.0.1", PORT, 3000);
            xclient.send("Hello, server.\n", 15);
            char buffer[BUFFER_SIZE] = {0};
            xclient.recv(buffer, BUFFER_SIZE);
            printf("Recv: %s\n", buffer);
            xclient.close();
        }
    }
    else
    {
        printf("Server start.\n");

        XTCP xserver;
        xserver.createSocket();
        // xserver.setBlock(false);

        /// create epoll;
        int epfd = epoll_create(256);

        xserver.bind(PORT);

        /// register epoll event

        epoll_event ev;
        ev.data.fd = xserver.getfd();
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epfd, EPOLL_CTL_ADD, xserver.getfd(), &ev);

        struct epoll_event event[20];

        char buffer[BUFFER_SIZE] = {0};
        const char *msg = "HTTP/1.1 200 OK\r\nContent-Length: 1\r\n\r\nX";
        int msg_len = strlen(msg);

        /// accpet client
        for (;;)
        {
            int count = epoll_wait(epfd, event, 20, 500);
            if (count <= 0)
                continue;
            for (int i = 0; i < count; ++i)
            {
                if (event[i].data.fd == xserver.getfd())
                {
                    XTCP client = xserver.accept();
                    if (!client.isVaild())
                    {
                        break;
                    }

                    /// register new epoll event
                    ev.data.fd = client.getfd();
                    ev.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client.getfd(), &ev);
                }
                else
                {
                    XTCP client;
                    client.setfd(event[i].data.fd);
                    client.recv(buffer, BUFFER_SIZE);
                    printf("Recv: %s\n", buffer);
                    client.send(msg, msg_len);

                    /// 客户端处理完毕，清理事件
                    epoll_ctl(epfd, EPOLL_CTL_DEL, client.getfd(), &ev);
                    client.close();
                }
            }
            // auto client = xserver.accept();
            // if (!client.isVaild()) {
            // 	break;
            // }
            //
            // TcpThread* tcpThread = new TcpThread;
            // tcpThread->client_xtcp_ = client;
            // std::thread th(&TcpThread::Main, tcpThread);
            // th.detach();
        }

        xserver.close();
    }

    return 0;
}

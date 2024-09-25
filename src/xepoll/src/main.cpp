#include "XTCP.h"

#include <cstring>

#define PORT 8080
#define BUFFER_SIZE 1024

#ifdef __LINUX__
#include <sys/epoll.h>
#elif __APPLE__
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#endif


int main(int argc, char *argv[])
{

    printf("Server start.\n");

    XTCP xserver;
    xserver.createSocket();
    // xserver.setBlock(false);

    /// create epoll;
#ifdef __LINUX__
    int epfd = epoll_create(256);
#elif __APPLE__
    int epid = kqueue();
#endif
    xserver.bind(PORT);

    /// register epoll event
#ifdef __LINUX__
    epoll_event ev;
    ev.data.fd = xserver.getfd();
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, xserver.getfd(), &ev);
    struct epoll_event event[20];
#elif __APPLE__
    struct kevent changes;
    EV_SET(&changes, xserver.getfd(), EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
    struct kevent event[20];
#endif

    char buffer[BUFFER_SIZE] = {0};
    const char *msg = "HTTP/1.1 200 OK\r\nContent-Length: 1\r\n\r\nX";
    int msg_len = strlen(msg);

    /// accpet client
    for (;;)
    {
#ifdef __LINUX__
        int count = epoll_wait(epfd, event, 20, 500);
#elif __APPLE__
        int count = kevent(epid, &changes, 0, &event, 20, 500);
#endif
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
#ifdef __LINUX__
                ev.data.fd = client.getfd();
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client.getfd(), &ev);
#elif __APPLE__
                changes.data.fd = client.getfd();
                EV_SET(&changes, xserver.getfd(), EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
#endif
            }
            else
            {
                XTCP client;
                client.setfd(event[i].data.fd);
                client.recv(buffer, BUFFER_SIZE);
                printf("Recv: %s\n", buffer);
                client.send(msg, msg_len);

                /// 客户端处理完毕，清理事件
#ifdef __LINUX__
                epoll_ctl(epfd, EPOLL_CTL_DEL, client.getfd(), &ev);
#elif __APPLE__
                EV_SET(&changes, xserver.getfd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
#endif
                client.close();
            }
        }
    }

    xserver.close();

    return 0;
}

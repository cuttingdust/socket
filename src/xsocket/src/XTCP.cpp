#include "XTCP.h"

#ifdef _WIN32
#include <winsock2.h>
#define socklen_t int
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h> /// For inet_addr
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> /// 包含 close 函数
#define closesocket close
#endif

#include <mutex>
#include <stdio.h>
#include <string.h>
#include <thread>

class XTCP::PImpl
{
public:
    PImpl(XTCP *owenr);
    ~PImpl();

public:
    XTCP *owenr_ = nullptr;
    int socked_fd_ = -1;

    char ip_[16];
    unsigned short port_ = -1;

    std::mutex mutex_; // 添加互斥锁
};

XTCP::PImpl::PImpl(XTCP *owenr) : owenr_(owenr) {}

XTCP::PImpl::~PImpl() = default;

XTCP::XTCP()
{
    impl_ = std::make_shared<PImpl>(this);

#ifdef _WIN32
    static std::once_flag flag;
    std::call_once(flag,
                   []()
                   {
                       WSADATA wsaData;
                       if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
                       {
                           printf("Failed to initialize Winsock.\n");
                       }
                   });
#endif
}

XTCP::~XTCP() {}

auto XTCP::createSocket() const -> int
{
    impl_->socked_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (impl_->socked_fd_ < 0)
    {
        printf("Failed to create socket.\n");
        return static_cast<int>(impl_->socked_fd_);
    }

    printf("Socket %d created.\n", impl_->socked_fd_);

    return static_cast<int>(impl_->socked_fd_);
}

auto XTCP::bind(unsigned int port) -> bool
{
    if (impl_->socked_fd_ < 0)
        createSocket();

    /// 绑定地址
    struct sockaddr_in sadder = {};
    sadder.sin_family = AF_INET;
    sadder.sin_addr.s_addr = INADDR_ANY; /// 接受任何地址
    sadder.sin_port = htons(port);

    if (::bind(impl_->socked_fd_, reinterpret_cast<struct sockaddr *>(&sadder), sizeof(sadder)) < 0)
    {
        printf("Bind failed.\n");
        return false;
    }

    printf("Bind to port %d\n", port);

    /// 开始监听
    if (::listen(impl_->socked_fd_, 10) < 0)
    {
        printf("Listen failed.\n");
        return false;
    }

    printf("Server listening on port %d\n", port);
    return true;
}

auto XTCP::accept() -> XTCP
{
    XTCP tcp;
    sockaddr_in cadder = {};
    socklen_t cadderlen = sizeof(cadder);

    // std::lock_guard<std::mutex> lock(impl_->mutex_); // 加锁

    auto client_fd = ::accept(impl_->socked_fd_, reinterpret_cast<sockaddr *>(&cadder), &cadderlen);
    if (client_fd < 0)
    {
        printf("Accept failed\n");
        return tcp;
    }
    printf("Accept Client_fd: %d\n", client_fd);

    tcp.impl_->socked_fd_ = client_fd;
    char *ip = inet_ntoa(cadder.sin_addr);
    strcpy(tcp.impl_->ip_, ip);
    tcp.impl_->port_ = ntohs(cadder.sin_port);
    printf("client IP is %s, port is %d\n", tcp.impl_->ip_, tcp.impl_->port_);

    return tcp;
}

auto XTCP::close() -> void
{
    if (impl_->socked_fd_ < 0)
        return;

    if (::closesocket(impl_->socked_fd_) < 0)
    {
        printf("Failed to close socket.\n");
        return;
    }
}

auto XTCP::recv(char *buf, int buf_size) -> int { return ::recv(impl_->socked_fd_, buf, buf_size, 0); }

auto XTCP::send(const char *buf, int buf_size) -> int
{
    int s = 0;
    while (s != buf_size)
    {
        s += ::send(impl_->socked_fd_, buf + s, buf_size - s, 0);
    }

    return s;
}

auto XTCP::isVaild() const -> bool { return impl_->socked_fd_ >= 0; }

auto XTCP::connect(const char *ip, unsigned short port, int timeoutms) -> bool
{
    if (impl_->socked_fd_ < 0)
        createSocket();

    struct sockaddr_in sadder = {};
    sadder.sin_family = AF_INET;
    sadder.sin_addr.s_addr = inet_addr(ip);
    sadder.sin_port = htons(port);
    setBlock(false);

    fd_set set;
    if (::connect(impl_->socked_fd_, reinterpret_cast<struct sockaddr *>(&sadder), sizeof(sadder)) < 0)
    {
        FD_ZERO(&set);
        FD_SET(impl_->socked_fd_, &set);

        timeval tm;
        tm.tv_sec = 0;
        tm.tv_usec = timeoutms * 1000;

        if (::select(impl_->socked_fd_ + 1, 0, &set, 0, &tm) <= 0)
        {
            printf("connect timeout or error!\n");
            printf("Connect %s:%d failed:%s\n", ip, port, strerror(errno));
            return false;
        }

        setBlock(true);
        printf("connect %s:%d success!\n", ip, port);
        return true;
    }

    strcpy(impl_->ip_, ip);
    impl_->port_ = port;
    printf("Connected to %s:%d\n", ip, port);
    return true;
}

auto XTCP::setBlock(bool is_block) -> bool
{
    if (impl_->socked_fd_ < 0)
    {
        printf("Socket is not created.\n");
        return false; // 返回失败状态
    }

#ifdef _WIN32
    u_long mode = is_block ? 0 : 1; // 0=阻塞, 1=非阻塞
    if (::ioctlsocket(impl_->socked_fd_, FIONBIO, &mode) != 0)
    {
        perror("ioctlsocket failed");
        return false; // 返回失败状态
    }
#else
    int flags = fcntl(impl_->socked_fd_, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl get failed");
        return false; // 返回失败状态
    }

    // 设置或清除非阻塞标志
    if (is_block)
    {
        flags &= ~O_NONBLOCK; // 清除非阻塞位
    }
    else
    {
        flags |= O_NONBLOCK; // 设置非阻塞位
    }

    // 更新套接字状态
    if (fcntl(impl_->socked_fd_, F_SETFL, flags) == -1)
    {
        perror("fcntl set failed");
        return false; // 返回失败状态
    }
#endif

    printf("Socket %d set to %s mode\n", impl_->socked_fd_, is_block ? "blocking" : "non-blocking");
    return true; // 返回成功状态
}

auto XTCP::getfd() const -> int { return impl_->socked_fd_; }

auto XTCP::setfd(int fd) -> void { impl_->socked_fd_ = fd; }

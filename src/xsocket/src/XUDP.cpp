#include "XUDP.h"

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
#define closesocket ::close
#endif

#include <mutex>
#include <stdio.h>
#include <string.h>
#include <thread>


class XUDP::PImpl
{
public:
    PImpl(XUDP *owenr);
    ~PImpl();

public:
    XUDP *owenr_ = nullptr;

    int socked_fd_ = -1;

    sockaddr_in *addr_ = 0;

    bool broad_model_ = false;
};

XUDP::PImpl::PImpl(XUDP *owenr) : owenr_(owenr) {}

XUDP::PImpl::~PImpl() = default;

XUDP::XUDP()
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

XUDP::~XUDP() = default;

auto XUDP::createSocket() -> int
{
    impl_->socked_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (impl_->socked_fd_ < 0)
    {
        printf("Failed to create socket.\n");
        return static_cast<int>(impl_->socked_fd_);
    }

    printf("Socket %d created.\n", impl_->socked_fd_);

    return static_cast<int>(impl_->socked_fd_);
}

auto XUDP::bind(unsigned short port) -> bool
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

    listen(impl_->socked_fd_, 10);

    printf("Server listening on port %d\n", port);
    return true;
}

auto XUDP::recv(char *buf, int bufsize) -> int
{
    if (impl_->socked_fd_ <= 0)
        return 0;
    if (impl_->addr_ == nullptr)
    {
        impl_->addr_ = new sockaddr_in();
    }
    socklen_t len = sizeof(sockaddr_in);
    int re = ::recvfrom(impl_->socked_fd_, buf, bufsize, 0, (sockaddr *)impl_->addr_, &len);
    return re;
}

auto XUDP::send(const char *buf, int buf_size) -> int
{
    if (impl_->socked_fd_ <= 0 || !impl_->addr_)
        return 0;

    socklen_t len = sizeof(sockaddr_in);
    int re = ::sendto(impl_->socked_fd_, buf, buf_size, 0, (sockaddr *)impl_->addr_, len);
    return re;
}

auto XUDP::close() -> void
{
    if (impl_->socked_fd_ <= 0)
        return;
    closesocket(impl_->socked_fd_);
    if (impl_->addr_)
    {
        delete impl_->addr_;
    }
    impl_->addr_ = nullptr;
    impl_->socked_fd_ = 0;
}

auto XUDP::getIP() const -> char *
{
    if (impl_->addr_ == nullptr)
        return nullptr;
    return inet_ntoa(impl_->addr_->sin_addr);
}

auto XUDP::setIP(const char *ip) -> void
{
    if (impl_->broad_model_)
    {
        return;
    }

    if (impl_->addr_ == nullptr)
    {
        impl_->addr_ = new sockaddr_in();
        impl_->addr_->sin_family = AF_INET;
    }

    impl_->addr_->sin_addr.s_addr = inet_addr(ip);
}

auto XUDP::getPort() const -> unsigned short
{
    if (impl_->addr_ == nullptr)
        return 0;
    return ntohs(impl_->addr_->sin_port);
}

auto XUDP::setPort(unsigned short port) -> void
{
    if (impl_->addr_ == nullptr)
    {
        impl_->addr_ = new sockaddr_in();
        impl_->addr_->sin_family = AF_INET;
    }

    impl_->addr_->sin_port = htons(port);
}

auto XUDP::setBroadModel(bool bOpenModel) -> void
{
    if (impl_->socked_fd_ < 0)
        return;

    impl_->broad_model_ = bOpenModel;
    int opt = bOpenModel ? 1 : 0;
    ::setsockopt(impl_->socked_fd_, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&opt), sizeof(opt));
    printf("Set broad model %s\n", bOpenModel ? "true" : "false");

    if (impl_->addr_ == nullptr)
    {
        impl_->addr_ = new sockaddr_in();
        impl_->addr_->sin_family = AF_INET;
    }
    impl_->addr_->sin_addr.s_addr = INADDR_BROADCAST;
}

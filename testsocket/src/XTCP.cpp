#include "XTCP.h"

#ifdef _WIN32
#include <winsock2.h>
#define socklen_t int
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // For inet_addr
#include <unistd.h> // 包含 close 函数
#define closesocket close
#endif

#include <stdio.h>
#include <string.h>
#include <thread>
#include <mutex>

class XTCP::PImpl
{
public:
	PImpl(XTCP* owenr);
	~PImpl();

public:
	XTCP* owenr_ = nullptr;
	SOCKET socked_fd_ = -1;

	char ip_[16];
	unsigned short port_ = -1;
};

XTCP::PImpl::PImpl(XTCP* owenr)
	: owenr_(owenr)
{
}

XTCP::PImpl::~PImpl() = default;


XTCP::XTCP()
{
	impl_ = std::make_shared<PImpl>(this);

#ifdef _WIN32
	static std::once_flag flag;
	std::call_once(flag, []()
		{
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				printf("Failed to initialize Winsock.\n");
			}
		});
#endif
}

XTCP::~XTCP()
{

}

auto XTCP::createSocket() const -> int
{
	impl_->socked_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (impl_->socked_fd_ < 0)
	{
		printf("Failed to create socket.\n");
#ifdef _WIN32
		WSACleanup(); ///  只有在Windows上才能调用
#endif
		return static_cast<int>(impl_->socked_fd_);
	}

	printf("Socket %llu created.\n", impl_->socked_fd_);

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

	if (::bind(impl_->socked_fd_, reinterpret_cast<struct sockaddr*>(&sadder), sizeof(sadder)) < 0)
	{
		printf("Bind failed.\n");
		return false;
	}

	printf("Bind to port %d\n", port);

	/// 开始监听
	if (listen(impl_->socked_fd_, 10) < 0)
	{
		printf("Listen failed.\n");
		return false;
	}

	printf("Server listening on port %d\n", port);
	return true;
}

auto XTCP::accepct() -> XTCP
{
	XTCP tcp;
	sockaddr_in cadder = {};
	socklen_t cadderlen = sizeof(cadder);
	auto client_fd = ::accept(impl_->socked_fd_, reinterpret_cast<sockaddr*>(&cadder), &cadderlen);
	if (client_fd < 0)
	{
		printf("Accept failed with error: %d\n", WSAGetLastError());
		return tcp;
	}
	printf("Accept Client_fd: %d\n", client_fd);

	tcp.impl_->socked_fd_ = client_fd;
	char* ip = inet_ntoa(cadder.sin_addr);
	strcpy(tcp.impl_->ip_, ip);
	tcp.impl_->port_ = ntohs(cadder.sin_port);
	printf("client IP is %s, port is %d\n", tcp.impl_->ip_, tcp.impl_->port_);

	return tcp;
}

auto XTCP::close() -> void
{
	if (impl_->socked_fd_ < 0)
		return;

	if (closesocket(impl_->socked_fd_) < 0)
	{
		printf("Failed to close socket.\n");
		return;
	}
}

auto XTCP::recv(char* buf, int buf_size) -> int
{
	return ::recv(impl_->socked_fd_, buf, buf_size, 0);
}

auto XTCP::send(const char* buf, int buf_size) -> int
{
	int s = 0;
	while (s != buf_size)
	{
		s += ::send(impl_->socked_fd_, buf + s, buf_size - s, 0);
	}

	return s;
}

auto XTCP::isVaild() const -> bool
{
	return impl_->socked_fd_ >= 0;
}

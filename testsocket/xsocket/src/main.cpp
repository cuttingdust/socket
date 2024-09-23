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

#include "XTCP.h"

#include <stdio.h>
#include <string.h>
#include <thread>

#define PORT 12345
#define BUFFER_SIZE 1024

class TcpThread
{
public:
	/// 线程函数
	void Main()
	{
		/// 处理客户端数据
		char buffer[BUFFER_SIZE] = { 0 };

		for (;;)
		{
			const int recvlen = client_xtcp_.recv(buffer, BUFFER_SIZE);
			if (recvlen <= 0) {
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


int main(int argc, char* argv[])
{
	XTCP xserver;
	auto socked_fd = xserver.createSocket();

	xserver.bind(PORT);

	/// 接受连接
	for (;;)
	{
		auto client = xserver.accepct();
		if (!client.isVaild())
		{
			break;
		}

		TcpThread* tcpThread = new TcpThread;
		tcpThread->client_xtcp_ = client;
		std::thread th(&TcpThread::Main, tcpThread);
		th.detach();
	}

	xserver.close();
}

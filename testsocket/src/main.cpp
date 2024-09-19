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
			const int recvlen = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
			if (recvlen <= 0) {
				printf("Client disconnected or error occurred.\n");
				break;
			}
			buffer[recvlen] = '\0';
			if (strstr(buffer, "quit") != NULL)
			{
				printf("Client quit.\n");
				send(client_fd, "quit success\n", 13, 0);
				break;
			}
			printf("Recv: %s\n", buffer);
			send(client_fd, "ok\n",3, 0); /// Echo back
		}

		if (closesocket(client_fd) < 0)
		{
			printf("Failed to close client socket.\n");
		}

		delete this;
	}
public:
	int client_fd = -1;
};


int main(int argc, char* argv[])
{
	/// 初始化
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to initialize Winsock.\n");
		return -1;
	}
#endif

	auto socked_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socked_fd < 0)
	{
		printf("Failed to create socket.\n");
#ifdef _WIN32
		WSACleanup(); ///  只有在Windows上才能调用
#endif
		return -2;
	}

	printf("Socket %d created.\n", socked_fd);

	/// 绑定地址
	struct sockaddr_in sadder = {};
	sadder.sin_family = AF_INET;
	sadder.sin_addr.s_addr = INADDR_ANY; /// 接受任何地址
	sadder.sin_port = htons(PORT);

	if (bind(socked_fd, reinterpret_cast<struct sockaddr*>(&sadder), sizeof(sadder)) < 0)
	{
		printf("Bind failed.\n");
		return -3;
	}

	printf("Bind to port %d\n", PORT);

	/// 开始监听
	if (listen(socked_fd, 10) < 0)
	{
		printf("Listen failed.\n");
		return -4;
	}

	printf("Server listening on port %d\n", PORT);

	/// 接受连接

	for (;;)
	{
		sockaddr_in cadder = {};
		socklen_t cadderlen = sizeof(cadder);
		int client_fd = accept(socked_fd, reinterpret_cast<struct sockaddr*>(&cadder), &cadderlen);
		if (client_fd < 0)
		{
			printf("Accept failed.\n");
			break;
		}
		printf("Accept Client_fd: %d\n", client_fd);
		char* ip = inet_ntoa(cadder.sin_addr);
		int port = ntohs(cadder.sin_port);
		printf("client IP is %s, port is %d\n", ip, port);

		TcpThread* tcpThread = new TcpThread;
		tcpThread->client_fd = client_fd;
		std::thread th(&TcpThread::Main, tcpThread);
		th.detach();
	}

	if (closesocket(socked_fd) < 0)
	{
		printf("Failed to close socket.\n");
	}

	/// 清理
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}

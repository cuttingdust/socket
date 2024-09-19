#ifdef _WIN32
#include <winsock2.h>
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

#define PORT 12345
#define BUFFER_SIZE 1024


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
	sockaddr_in cadder = {};
#ifdef _WIN32
	int cadderlen;
#else
	socklen_t cadderlen;
#endif

	cadderlen = sizeof(cadder);
	int client_fd = accept(socked_fd, reinterpret_cast<struct sockaddr*>(&cadder), &cadderlen);
	if (client_fd < 0)
	{
		printf("Accept failed.\n");
	}
	printf("Accept Client_fd: %d\n", client_fd);
	char* ip = inet_ntoa(cadder.sin_addr);
	int port = ntohs(cadder.sin_port);
	printf("client IP is %s, port is %d\n", ip, port);

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
			send(client_fd, "quit\n", 4, 0);
			break;
		}
		printf("Recv: %s\n", buffer);
		send(client_fd, buffer, recvlen, 0); /// Echo back
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

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

#include <iostream>
#include <cstring>

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

	// while (true)
	// {
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
	std::cout << "Accept Client_fd: " << client_fd << std::endl;
	char* cIp = inet_ntoa(cadder.sin_addr);
	int cPort = ntohs(cadder.sin_port);
	printf("client IP is %s, port is %d\n", cIp, cPort);

	/// 处理客户端数据
	char buffer[BUFFER_SIZE] = { 0 };
	while (true)
	{
		int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
		if (bytes_received <= 0) {
			std::cerr << "Client disconnected or error occurred." << std::endl;
			break;
		}
		buffer[bytes_received] = '\0';
		if (strstr(buffer, "quit") != nullptr)
		{
			std::cout << "Client quit." << std::endl;
			break;
		}
		std::cout << "Recv: " << std::string(buffer, bytes_received) << std::endl;
		send(client_fd, buffer, bytes_received, 0); /// Echo back
	}


	if (closesocket(socked_fd) < 0)
	{
		std::cerr << "Failed to close socket." << std::endl;
	}
	// }

	/// 清理
#ifdef _WIN32
	WSACleanup();
#endif

	std::cout << "Hello World" << std::endl;
	return 0;
}

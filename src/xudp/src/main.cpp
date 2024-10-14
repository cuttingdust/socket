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

#include <cstring>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

#define BROAD_MODEL


int main(int argc, char *argv[])
{
#ifdef WIN32
    WSADATA ws;
    WSAStartup(MAKEWORD(2, 2), &ws);
#endif

    /// 客户端
    if (argc > 1)
    {
        if (strcmp(argv[1], "client") == 0)
        {
            printf("Client start.\n");

            int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
            if (sock <= 0)
            {
                printf("Create socket failed.\n");
                return -1;
            }

#ifdef BROAD_MODEL
            int opt = 1;
            ::setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
#endif


            sockaddr_in saddr;
            saddr.sin_family = AF_INET;
            saddr.sin_port = htons(PORT);

#ifdef BROAD_MODEL
            saddr.sin_addr.s_addr = INADDR_BROADCAST;
#else
            saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // htonl(0);
#endif
            int len = ::sendto(sock, "12345", 6, 0, (sockaddr *)&saddr, sizeof(saddr));
            printf("Sendto size is %d\n", len);

            char buf[1024] = {0};
            ::recvfrom(sock, buf, sizeof(buf) - 1, 0, 0, 0);
            printf("%s\n", buf);
        }
    }
    else /// 服务端
    {
        printf("Server start.\n");

        int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (sock <= 0)
        {
            printf("Create socket failed.\n");
            return -1;
        }

        sockaddr_in saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(PORT);
        saddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (::bind(sock, (sockaddr *)&saddr, sizeof(saddr)) < 0)
        {
            printf("Bind port %d failed.\n", PORT);
            return -2;
        }

        printf("Bind port %d success.\n", PORT);

        ::listen(sock, 10);
        sockaddr_in client;
        socklen_t len = sizeof(client);
        char buffer[BUFFER_SIZE] = {0};
        int re_len = ::recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr *)&client, &len);
        if (re_len <= 0)
        {
            printf("RecvFrom failed.\n");
            return -3;
        }
        printf("Recv %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        buffer[re_len] = '\0';
        printf("%s\n", buffer);
        ::sendto(sock, "67890", 5, 0, (sockaddr *)&client, sizeof(client));
    }
    getchar();
}

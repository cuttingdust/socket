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

#include "XUDP.h"

#include <cstring>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

// #define BROAD_MODEL


int main(int argc, char *argv[])
{

    /// 客户端
    if (argc > 1)
    {
        if (strcmp(argv[1], "client") == 0)
        {
            printf("Client start.\n");
            XUDP udp_client;


            int sock = udp_client.createSocket();
            if (sock <= 0)
            {
                return -1;
            }

#ifdef BROAD_MODEL
            int opt = 1;
            ::setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
#endif


#ifdef BROAD_MODEL
            sockaddr_in saddr;
            saddr.sin_family = AF_INET;
            saddr.sin_port = htons(PORT);
            saddr.sin_addr.s_addr = INADDR_BROADCAST;
#else
            udp_client.setIP("127.0.0.1");
            udp_client.setPort(PORT);
#endif
            auto len = udp_client.send("12345", 6);
            printf("Sendto size is %d\n", len);

            char buf[1024] = {0};
            udp_client.recv(buf, sizeof(buf) - 1);
            printf("%s\n", buf);
        }
    }
    else /// 服务端
    {
        printf("Server start.\n");
        XUDP udp_server;
        auto fd = udp_server.createSocket();
        if (fd < 0)
        {
            return -1;
        }

        if (udp_server.bind(PORT) < 0)
        {
            return -2;
        }

        char buffer[BUFFER_SIZE] = {0};
        int re_len = udp_server.recv(buffer, sizeof(buffer));
        if (re_len <= 0)
        {
            printf("RecvFrom failed.\n");
            return -3;
        }
        printf("Recv %s:%d\n", udp_server.getIP(), udp_server.getPort());

        buffer[re_len] = '\0';
        printf("%s\n", buffer);
        udp_server.send("Hello, I am server.", 20);
    }
    getchar();
}

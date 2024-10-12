#include "XTCP.h"

#include <cstring>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

class TcpThread
{
public:
    /// 线程函数
    void Main()
    {
        /// 处理客户端数据
        char buffer[BUFFER_SIZE] = {0};

        for (;;)
        {
            const int recvlen = client_xtcp_.recv(buffer, BUFFER_SIZE);
            if (recvlen <= 0)
            {
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


int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "client") == 0)
        {
            printf("Client start.\n");
            
        }
    }
    else
    {
        printf("Server start.\n");

        
    }
}

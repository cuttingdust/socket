#include "XTCP.h"

#include <cstring>
#include <string>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 10000

class HTTPThread
{
public:
    /// 线程函数
    void Main()
    {
        /// 处理客户端数据
        char buffer[BUFFER_SIZE] = {0};


        {
            /// Accept HTTP client requests
            const int recvlen = client_xtcp_.recv(buffer, BUFFER_SIZE - 1);
            if (recvlen <= 0)
            {
                printf("Client disconnected or error occurred.\n");
                close();
                return;
            }
            buffer[recvlen] = '\0';
            printf("=======recv=========\n%s===================\n", buffer);

            /// Responding to HTTP GET requests

            std::string rmsg = "";
            rmsg += "HTTP/1.1 200 OK\r\n";
            rmsg += "Server: XHttp\r\n";
            rmsg += "Content-Type: text/html\r\n";
            rmsg += "Content-Length: ";
            rmsg += "10";
            rmsg += "\r\n\r\n";
            rmsg += "0123456789";

            /// Send message header
            const int sendSize = client_xtcp_.send(rmsg.c_str(), rmsg.size());
            printf("sendsize = %d\n", sendSize);
            printf("=======send=========\n%s===================\n", rmsg.c_str());
        }

        close();
    }

    void close()
    {
        client_xtcp_.close();
        delete this;
    }

public:
    XTCP client_xtcp_;
};


int main(int argc, char *argv[])
{

    printf("Server start.\n");

    XTCP xserver;
    xserver.createSocket();
    xserver.setBlock(true);
    xserver.bind(PORT);

    /// 接受连接
    for (;;)
    {
        auto client = xserver.accept();
        if (!client.isVaild())
        {
            break;
        }

        HTTPThread *tcpThread = new HTTPThread;
        tcpThread->client_xtcp_ = client;
        std::thread th(&HTTPThread::Main, tcpThread);
        th.detach();
    }

    xserver.close();
}

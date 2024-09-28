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
            // GET /favicon.ico HTTP/1.1
            // Host: 127.0.0.1:8080
            // Connection: keep-alive
            // sec-ch-ua-platform: "Windows"
            // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko)
            // Chrome/129.0.0.0 Safari/537.36 sec-ch-ua: "Google Chrome";v="129", "Not=A?Brand";v="8",
            // "Chromium";v="129" sec-ch-ua-mobile: ?0 Accept:
            // image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8 Sec-Fetch-Site: same-origin
            // Sec-Fetch-Mode: no-cors
            // Sec-Fetch-Dest: image
            // Referer: http://127.0.0.1:8080/
            // Accept-Encoding: gzip, deflate, br, zstd
            // Accept-Language: zh-CN,zh;q=0.9

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

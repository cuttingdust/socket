#include "XHTTPServer.h"
// #include "XTCP.h"

#ifdef __linux__
#include <signal.h>
#endif

#include <stdio.h>
// #include <cstring>
// #include <regex>
// #include <string>
// #include <thread>

#define PORT 8080
// #define BUFFER_SIZE 10240
//
//
// class HTTPThread
// {
// public:
//     /// 线程函数
//     void Main()
//     {
//         /// 处理客户端数据
//         char buffer[BUFFER_SIZE] = {0};
//         for (;;)
//         {
//             /// Accept HTTP client requests
//             const int recvlen = client_xtcp_.recv(buffer, BUFFER_SIZE - 1);
//             if (recvlen <= 0)
//             {
//                 printf("Client disconnected or error occurred.\n");
//                 close();
//                 return;
//             }
//             buffer[recvlen] = '\0';
//             printf("=======recv=========\n%s===================\n", buffer);
//
//             /// Responding to HTTP GET requests
//             // GET /favicon.ico HTTP/1.1
//             // Host: 127.0.0.1:8080
//             // Connection: keep-alive
//             // sec-ch-ua-platform: "Windows"
//             // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko)
//             // Chrome/129.0.0.0 Safari/537.36 sec-ch-ua: "Google Chrome";v="129", "Not=A?Brand";v="8",
//             // "Chromium";v="129" sec-ch-ua-mobile: ?0 Accept:
//             // image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8 Sec-Fetch-Site: same-origin
//             // Sec-Fetch-Mode: no-cors
//             // Sec-Fetch-Dest: image
//             // Referer: http://127.0.0.1:8080/
//             // Accept-Encoding: gzip, deflate, br, zstd
//             // Accept-Language: zh-CN,zh;q=0.9
//
//             std::string src = buffer;
//             // /   /index.html /ff
//             // std::string pattern = "^([A-Z]+) (.+) HTTP/1"; /// [0] [1] [2]
//             std::string pattern = "^([A-Z]+) /([a-zA-Z0-9]*([.][a-zA-Z]*)?)[?]?(.*) HTTP/1";
//             std::regex r(pattern);
//             std::smatch mas;
//             regex_search(src, mas, r);
//             if (mas.size() == 0)
//             {
//                 printf("%s failed!\n", pattern.c_str());
//                 close();
//                 return;
//             }
//
//             std::string type = mas[1];
//             std::string path = "/";
//             path += mas[2];
//             std::string filetype = mas[3];
//             std::string query = mas[4];
//             if (filetype.size() > 0)
//                 filetype = filetype.substr(1, filetype.size() - 1);
//             printf("type:[%s]\npath:[%s]\nfiletype:[%s]\nquery:[%s]\n", type.c_str(), path.c_str(), filetype.c_str(),
//                    query.c_str());
//
//             if (type != "GET")
//             {
//                 printf("Not GET!!!\n");
//                 close();
//                 return;
//             }
//
//             if (path == "/testsocket")
//             {
//                 std::string rmsg = "";
//                 rmsg += "HTTP/1.1 200 OK\r\n";
//                 rmsg += "Server: XHttp\r\n";
//                 rmsg += "Content-Type: text/html\r\n";
//                 rmsg += "Content-Length: ";
//                 rmsg += "10";
//                 rmsg += "\r\n\r\n";
//                 rmsg += "0123456789";
//
//                 /// Send message header
//                 const int sendSize = client_xtcp_.send(rmsg.c_str(), rmsg.size());
//                 printf("sendsize = %d\n", sendSize);
//                 printf("=======send=========\n%s===================\n", rmsg.c_str());
//                 return;
//             }
//
//
//             std::string filename = path;
//             if (path == "/")
//             {
//                 filename = "/index.html";
//             }
//
//             std::string filepath = "www";
//             filepath += filename;
//
//             // php-cgi www/index.php id=1 name=xcj >  www/index.php.html
//             if (filetype == "php")
//             {
//                 std::string cmd = "php-cgi ";
//                 cmd += filepath;
//                 cmd += " ";
//                 // query id=1&name=xcj
//                 //  id=1 name=xcj
//                 for (int i = 0; i < query.size(); i++)
//                 {
//                     if (query[i] == '&')
//                         query[i] = ' ';
//                 }
//                 cmd += query;
//
//                 cmd += " > ";
//                 filepath += ".html";
//                 cmd += filepath;
//
//
//                 printf("%s\n", cmd.c_str());
//                 system(cmd.c_str());
//             }
//
//             FILE *fp = fopen(filepath.c_str(), "rb");
//             if (fp == NULL)
//             {
//                 printf("open file %s failed!\n", filepath.c_str());
//                 close();
//                 return;
//             }
//             /// 获取文件大小
//             fseek(fp, 0, SEEK_END);
//             int filesize = ftell(fp);
//             fseek(fp, 0, 0);
//             printf("file size is %d\n", filesize);
//
//             if (filetype == "php")
//             {
//                 char c = 0;
//                 //\r\n\r\n
//                 int headsize = 0;
//                 /// 逐行读取文件
//                 char line[256]; /// 用于存储每行读取的数据
//                 while (fgets(line, sizeof(line), fp))
//                 {
//                     headsize += strlen(line); /// 计算当前行的大小
//                     if (strcmp(line, "\r\n") == 0 || strcmp(line, "\n") == 0)
//                     {
//                         break; /// 找到空行，表示头部结束
//                     }
//                 }
//                 filesize = filesize - headsize;
//             }
//
//             /// 回应http GET请求
//             /// 消息头
//             std::string rmsg = "";
//             rmsg = "HTTP/1.1 200 OK\r\n";
//             rmsg += "Server: XHttp\r\n";
//             rmsg += "Content-Type: text/html\r\n";
//             rmsg += "Content-Length: ";
//             char bsize[128] = {0};
//             sprintf(bsize, "%d", filesize);
//             rmsg += bsize;
//             // rmsg +=
//             // rmsg += "10\r\n";
//             rmsg += "\r\n\r\n";
//             // rmsg += "0123456789";
//
//             /// 发送消息头
//             int sendSize = client_xtcp_.send(rmsg.c_str(), rmsg.size());
//             printf("sendsize = %d\n", sendSize);
//             printf("=======send=========\n%s\n=============\n", rmsg.c_str());
//
//             /// 发送正文
//             for (;;)
//             {
//                 int len = fread(buffer, 1, sizeof(buffer), fp);
//                 if (len <= 0)
//                     break;
//                 int re = client_xtcp_.send(buffer, len);
//                 if (re <= 0)
//                     break;
//             }
//         }
//         close();
//     }
//
//     void close()
//     {
//         client_xtcp_.close();
//         delete this;
//     }
//
// public:
//     XTCP client_xtcp_;
// };


int main(int argc, char *argv[])
{
#ifdef __linux__
    signal(SIGPIPE, SIG_IGN);
#endif

    printf("Server start.\n");

    XHTTPServer server;
    server.start(PORT);

    // XTCP xserver;
    // xserver.createSocket();
    // xserver.setBlock(true);
    // xserver.bind(PORT);
    //
    // /// 接受连接
    // for (;;)
    // {
    //     auto client = xserver.accept();
    //     if (!client.isVaild())
    //     {
    //         break;
    //     }
    //
    //     HTTPThread *tcpThread = new HTTPThread;
    //     tcpThread->client_xtcp_ = client;
    //     std::thread th(&HTTPThread::Main, tcpThread);
    //     th.detach();
    // }
    //
    // xserver.close();

    getchar();
    return 0;
}

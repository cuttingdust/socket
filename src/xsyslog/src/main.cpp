#include "XUDP.h"
#include <stdio.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#include <iostream>
#include <regex>
#include <string>

int main(int argc, char *argv[])
{
    printf("XSysLog Server start!\n");

    XUDP syslog;
    if (!syslog.bind(PORT))
    {
        return -1;
    }
    char buf[2000] = {0};
    for (;;)
    {
        /// 接收linux发送的日志
        int len = syslog.recv(buf, sizeof(buf));
        if (len <= 0)
        {
            continue;
        }
        buf[len] = '\0';
        // printf("Received: %s\n", buf);

        /// <12>Oct 15 15:42:27 DSM918 Connection: User [hildness] from [240e:390:6e1:93a0:7c3b:9be8:5244:89b7] failed
        /// to sign in to [DSM] via [password] due to authorization failure.
        std::string src = buf;
        ///  正则表达式
        std::regex logRegex(
            R"(<\d+>(\s*(.*?)\s\d+ \d+:\d+:\d+) ([a-zA-Z0-9]+) Connection: User \[([a-zA-Z0-9]+)] from \[(\b(?:(?:[0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}|(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(?:\.(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){2})?\b)] failed to sign in to \[[a-zA-Z0-9]+] via \[password] due to authorization failure.)");

        std::smatch match;
        if (std::regex_search(src, match, logRegex))
        {
            std::string timestamp = match[1]; /// 时间
            std::string device = match[2]; /// 设备名称
            std::string user = match[3]; /// 用户名
            std::string ip = match[4]; /// IP 地址

            /// 输出结果
            std::cout << "Timestamp: " << timestamp << std::endl;
            std::cout << "Device: " << device << std::endl;
            std::cout << "User: " << user << std::endl;
            std::cout << "IP: " << ip << std::endl;
        }
        else
        {
            std::cout << "No match found." << std::endl;
        }
    }
    return 0;
}

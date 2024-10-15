#include "XUDP.h"
#include <stdio.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#include <iostream>
#include <regex>
#include <string>
using namespace std;
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
        printf("Received: %s\n", buf);

        // // Failed password for xcj from 192.168.3.76 port 16939 ssh2
        // string src = buf;
        // string p = "Failed password for ([a-zA-Z0-9]+) from ([0-9.]+)";
        // regex r(p);
        // smatch mas;
        // // 分析日志内容
        // regex_search(src, mas, r);
        // if (mas.size() > 0)
        // {
        //     cout << "Warning: User " << mas[1] << "@" << mas[2] << "login failed!" << endl;
        // }
    }
    return 0;
}

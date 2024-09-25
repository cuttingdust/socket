#include "XTCP.h"

#include <cstring>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

class TcpThread {
public:
  /// 线程函数
  void Main() {
    /// 处理客户端数据
    char buffer[BUFFER_SIZE] = {0};

    for (;;) {
      const int recvlen = client_xtcp_.recv(buffer, BUFFER_SIZE);
      if (recvlen <= 0) {
        printf("Client disconnected or error occurred.\n");
        break;
      }
      buffer[recvlen] = '\0';
      if (strstr(buffer, "quit") != NULL) {
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

#include <sys/epoll.h>

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "client") == 0) {
      printf("Client start.\n");
      XTCP xclient;
      xclient.createSocket();
      // xclient.setBlock(false);

      xclient.connect("127.0.0.1", PORT, 3000);
      xclient.send("Hello, server.\n", 15);
      char buffer[BUFFER_SIZE] = {0};
      xclient.recv(buffer, BUFFER_SIZE);
      printf("Recv: %s\n", buffer);
      client.close();
    }
  } else {
    printf("Server start.\n");

    XTCP xserver;
    // xserver.createSocket();
    // xserver.setBlock(false);

    /// create epoll;
    int epfd = epoll_create(256);
    xserver.bind(PORT);

    epoll_event ev;
    ev.data.fd = xserver.

                 /// 接受连接
                 for (;;) {
      auto client = xserver.accepct();
      if (!client.isVaild()) {
        break;
      }

      TcpThread *tcpThread = new TcpThread;
      tcpThread->client_xtcp_ = client;
      std::thread th(&TcpThread::Main, tcpThread);
      th.detach();
    }

    xserver.close();
  }
}

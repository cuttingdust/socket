/**
 * @file   XHTTPServer.h
 * @brief  HTTP·þÎñÆ÷
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-10-09
 */

#ifndef XHTTPSERVER_H
#define XHTTPSERVER_H

#include "XSocket_Global.h"

class XSOCKET_EXPORT XHTTPServer
{
public:
    XHTTPServer();
    virtual ~XHTTPServer();

public:
    auto start(unsigned short port) -> bool;

    auto stop() -> void;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XHTTPSERVER_H

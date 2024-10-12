/**
 * @file   XHTTPClient.h
 * @brief
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-10-10
 */

#ifndef XHTTPCLIENT_H
#define XHTTPCLIENT_H

#include "XTCP_Global.h"

class XTCP;

class XTCP_EXPORT XHTTPClient
{
public:
    XHTTPClient();
    virtual ~XHTTPClient();

public:
    auto start(XTCP client) -> bool;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};


#endif // XHTTPCLIENT_H

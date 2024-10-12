#include "XHTTPClient.h"
#include "XHTTPResponse.h"

#include "XTCP.h"

#include <thread>

class XHTTPClient::PImpl
{
public:
    PImpl(XHTTPClient *owenr);
    ~PImpl();
    auto recvData() -> void;

public:
    XHTTPClient *owenr_ = nullptr;

    XTCP client_xtcp_;
    XHTTPResponse res_;
};

XHTTPClient::PImpl::PImpl(XHTTPClient *owenr) : owenr_(owenr) {}

XHTTPClient::PImpl::~PImpl() = default;

auto XHTTPClient::PImpl::recvData() -> void
{
    char buffer[10240] = {0};
    for (;;)
    {
        int len = client_xtcp_.recv(buffer, sizeof(buffer) - 1);
        if (len <= 0)
        {
            break;
        }
        buffer[len] = '\0';

        if (!res_.setRequest(buffer))
        {
            break;
        }
        std::string head = res_.getHead();
        if (client_xtcp_.send(head.c_str(), head.size()) <= 0)
        {
            break;
        }
        int size = sizeof(buffer);
        bool error = false;
        for (;;)
        {
            int len = res_.read(buffer, size);
            if (len < 0)
            {
                error = true;
                break;
            }
            if (len == 0)
                break;
            if (client_xtcp_.send(buffer, len) <= 0)
            {
                error = true;
                break;
            }
        }
    }
    client_xtcp_.close();
    delete owenr_;
}

XHTTPClient::XHTTPClient() { impl_ = std::make_shared<PImpl>(this); }

XHTTPClient::~XHTTPClient() {}

auto XHTTPClient::start(XTCP client) -> bool
{
    if (!client.isVaild())
    {
        return false;
    }
    impl_->client_xtcp_ = client;
    std::thread(&XHTTPClient::PImpl::recvData, impl_).detach();
    return true;
}

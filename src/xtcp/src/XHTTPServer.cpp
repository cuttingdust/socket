#include "XHTTPServer.h"
#include "XHTTPClient.h"
#include "XTCP.h"
#include <thread>

class XHTTPServer::PImpl
{
public:
    PImpl(XHTTPServer *owenr);
    ~PImpl();

public:
    auto acceptData() -> void;

public:
    XHTTPServer *owenr_ = nullptr;
    XTCP server_xtcp_;
    std::atomic_bool isexit = false;
};

XHTTPServer::PImpl::PImpl(XHTTPServer *owenr) : owenr_(owenr) {}

XHTTPServer::PImpl::~PImpl() = default;

auto XHTTPServer::PImpl::acceptData() -> void
{
    while (!isexit)
    {
        auto client = server_xtcp_.accept();
        if (client.getfd() < 0)
        {
            continue;
        }
        XHTTPClient *th = new XHTTPClient();
        th->start(client);
    }
    return;
}


XHTTPServer::XHTTPServer() { impl_ = std::make_shared<PImpl>(this); }

XHTTPServer::~XHTTPServer() {}

auto XHTTPServer::start(unsigned short port) -> bool
{
    auto fd = impl_->server_xtcp_.createSocket();
    if (fd < 0 || !impl_->server_xtcp_.bind(port))
    {
        printf("Failed to bind port %d.\n", port);
        return false;
    }

    std::thread sth(&XHTTPServer::PImpl::acceptData, impl_);
    sth.detach();

    return true;
}

auto XHTTPServer::stop() -> void { impl_->isexit = true; }

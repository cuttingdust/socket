/**
 * @file   XUDP.h
 * @brief
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-10-14
 */

#ifndef XUDP_H
#define XUDP_H

#include "XSocket_Global.h"

class XSOCKET_EXPORT XUDP
{
public:
    XUDP();
    virtual ~XUDP();

public:
    /// \brief 创建套接字
    /// \return
    auto createSocket() -> int;

    /// \brief 绑定端口
    /// \param port
    auto bind(unsigned short port) -> bool;

    /// \brief 接受数据
    /// \param buf
    /// \param bufsize
    /// \return
    auto recv(char *buf, int bufsize) -> int;

    /// \brief 发送数据
    /// \param buf 数据
    /// \param buf_size 发送的最大大小
    /// \return 发送出去的大小
    auto send(const char *buf, int buf_size) -> int;

    /// \brief 关闭socket 套接字
    auto close() -> void;

    /// \brief 获得IP
    /// \return
    auto getIP() const -> char *;

    /// \brief 设置IP
    /// \param ip
    auto setIP(const char *ip) -> void;

    /// \brief 获得端口
    /// \return
    auto getPort() const -> unsigned short;

    /// \brief 设置端口
    /// \param port
    auto setPort(unsigned short port) -> void;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};


#endif // XUDP_H

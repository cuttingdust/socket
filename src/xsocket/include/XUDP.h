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
    /// \brief �����׽���
    /// \return
    auto createSocket() -> int;

    /// \brief �󶨶˿�
    /// \param port
    auto bind(unsigned short port) -> bool;

    /// \brief ��������
    /// \param buf
    /// \param bufsize
    /// \return
    auto recv(char *buf, int bufsize) -> int;

    /// \brief ��������
    /// \param buf ����
    /// \param buf_size ���͵�����С
    /// \return ���ͳ�ȥ�Ĵ�С
    auto send(const char *buf, int buf_size) -> int;

    /// \brief �ر�socket �׽���
    auto close() -> void;

    /// \brief ���IP
    /// \return
    auto getIP() const -> char *;

    /// \brief ����IP
    /// \param ip
    auto setIP(const char *ip) -> void;

    /// \brief ��ö˿�
    /// \return
    auto getPort() const -> unsigned short;

    /// \brief ���ö˿�
    /// \param port
    auto setPort(unsigned short port) -> void;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};


#endif // XUDP_H

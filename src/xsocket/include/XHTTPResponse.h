/**
 * @file   XHTTPResponse.h
 * @brief
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-10-10
 */

#ifndef XHTTPRESPONSE_H
#define XHTTPRESPONSE_H

#include <memory>
#include <string>

class XHTTPResponse
{
public:
    XHTTPResponse();
    virtual ~XHTTPResponse();

public:
    /// \brief ������Ӧ������
    /// \param request
    /// \return
    auto setRequest(const std::string &request) -> bool;

    /// \brief ���ͷ�������Ϣ
    /// \return
    auto getHead() -> std::string const;

    /// \brief ��ȡ�ļ�
    /// \param buf
    /// \param bufsize
    /// \return
    auto read(char *buf, int bufsize) -> int;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XHTTPRESPONSE_H

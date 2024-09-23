/**
 * @file   XTCP.h
 * @brief
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-09-19
 */

#ifndef XTCP_H
#define XTCP_H


#include <memory>

class XTCP
{
public:
	XTCP();
	virtual ~XTCP();

public:
	/// \brief 创建socket 套接字
	/// \return 
	auto createSocket() const -> int;

	/// \brief 绑定端口
	/// \param port 
	/// \return 
	auto bind(unsigned int port) -> bool;

	/// \brief 接受链接
	/// \return 客户端对象
	auto accepct() -> XTCP;

	/// \brief 关闭socket 套接字
	auto close() -> void;

	/// \brief 接受数据
	/// \param buf 数据
	/// \param buf_size 数据最大大小 
	/// \return 接受到的大小
	auto recv(char* buf, int buf_size) -> int;

	/// \brief 发送数据
	/// \param buf 数据
	/// \param buf_size 发送的最大大小 
	/// \return 发送出去的大小
	auto send(const char* buf, int buf_size) -> int;

	/// \brief 是否有效
	/// \return 
	auto isVaild() const -> bool;
private:
	class PImpl;
	std::shared_ptr<PImpl> impl_;
};

#endif // XTCP_H


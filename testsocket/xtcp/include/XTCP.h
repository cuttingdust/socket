/**
 * @file   XTCP.h
 * @brief
 *
 * Detailed description if necessary.
 * tcp �����
 *
 * @author 31667
 * @date   2024-09-19
 */

#ifndef XTCP_H
#define XTCP_H

#include "XTCP_Global.h"

class XTCP_EXPORT XTCP
{
public:
	XTCP();
	virtual ~XTCP();

public:
	/// \brief ����socket �׽���
	/// \return 
	auto createSocket() const -> int;

	/// \brief �󶨶˿�
	/// \param port 
	/// \return 
	auto bind(unsigned int port) -> bool;

	/// \brief ��������
	/// \return �ͻ��˶���
	auto accepct() -> XTCP;

	/// \brief �ر�socket �׽���
	auto close() -> void;

	/// \brief ��������
	/// \param buf ����
	/// \param buf_size ��������С 
	/// \return ���ܵ��Ĵ�С
	auto recv(char* buf, int buf_size) -> int;

	/// \brief ��������
	/// \param buf ����
	/// \param buf_size ���͵�����С 
	/// \return ���ͳ�ȥ�Ĵ�С
	auto send(const char* buf, int buf_size) -> int;

	/// \brief �Ƿ���Ч
	/// \return 
	auto isVaild() const -> bool;

	/// \brief ����TCP������
	/// \param ip 
	/// \param port 
	/// \return 
	auto connect(const char* ip, unsigned short port, int timeoutms = 1000) -> bool;

	/// \brief �Ƿ��������� 
	/// \param is_block
  /// \return 
	auto setBlock(bool is_block) -> bool;

private:
	class PImpl;
	std::shared_ptr<PImpl> impl_;
};

#endif // XTCP_H


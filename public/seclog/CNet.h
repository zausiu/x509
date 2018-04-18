/***********************************************************************
 * File : CUdpNet.h
 * Brief: 封装UDP通讯的客户端常用功能
 * 
 * History
 * ----------------------------------
 * 2008-11-11	arrowgu   1.0	created
 * 
 ***********************************************************************
 */

#ifndef CNET_H_
#define CNET_H_

#include <string>

namespace SEC
{
namespace LOG
{

using std::string;


class CNet
{
public:
	CNet();
	virtual ~CNet(); 

	void Close();
	int	 GetSocketFd() { return m_iSockfd; }

	bool IsValid() { return m_iSockfd > 0; }
	const string& LastErr() { return m_sErr; }

	const string& GetPeerIp() { return m_sPeerIp; }
	uint16_t GetPeerPort() { return m_wPeerPort; }

	static const string& GetLocalIp();

	bool SetBlockMode(bool bBlock);
	bool GetBlockMode(bool& bBlock);
	
protected:
	const string& _LastSysErr();
	
protected:
	string m_sErr;
	int	   m_iSockfd;

	string m_sPeerIp;
	uint16_t m_wPeerPort;

	string m_sLastSysErr;

private:
	static string s_sLocalIp;
};

}
}

#endif


/***********************************************************************
 * File : CLogNetDriver.h
 * Brief: ����������־��ʵ����
 * 
 * History
 * ----------------------------------
 * 2008-11-11	arrowgu   1.0	created
 * 
 ***********************************************************************
 */


#ifndef CLOGNETDRIVER_H_
#define CLOGNETDRIVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CLog.h"
#include "CLogDriver.h"
#include "CUdpNet.h"

using std::string;
using namespace SEC::LOG;

namespace SEC
{

/**
 * д������־��ʵ����
 */
class CLogNetDriver: public CLogDriver
{
public:
	CLogNetDriver();
	virtual ~CLogNetDriver();

	/**
	 * �����ļ���־������
	 * 
	 * @param config ��ϸ�����ò���
	 * 
	 * @return ���ز����Ƿ�ɹ�
	 * 		true - �ɹ�
	 * 		false - ʧ��
	 */
	bool   SetConfig(const CNetLogConfig &config);
		
	/**
	 * ����д��־�ӿ�
	 *
	 * @param [in]	sUrl  url�ַ���
	 *
	 * @return �����Ƿ�ɹ�
	 *		true - �ɹ�
	 *		false - ʧ��
	 * 
	 */
	virtual bool Write(const char* szLogInfo, const char*szPrefix, const char* szAppend);	
	
	/**
	 * �����Ƿ�׼��д����
	 */
	virtual bool IsReady();

public:
	static CLogDriver* AllocDriver(const CNetLogConfig &config);

private:
	bool iFlush();	
	bool iSendLogData(uint8_t *pLog, int iLogLen);	
	bool iSendLogSegment(uint8_t *pLog, int iLogLen);
	bool iGetLogTimeStr(string &sTime);
	
private:
	enum {MAX_LOG_NET_DRIVERS = 32};

#pragma	pack(1)
	struct LogPkg
	{
		char 		STX;
		uint16_t 	ushLen;
		uint32_t	dwCmd;
		uint8_t		data[0];
	};
#pragma	pack()

	
	/**
	 * �ļ����������pool
	 */
	static CLogNetDriver   g_aLogNetDrivers[MAX_LOG_NET_DRIVERS];
	static int			   g_iDriverCnt;

private:
	CNetLogConfig 	m_config;
	CUdpNet 		m_udpNet;

	char			*m_pLogBuf;
	int				m_iLogBufLen;
	int				m_iLogBufMaxLen;
	
	//net pkg buff
	uint8_t 		*m_pPkgBuff;
	int 			m_iPkgBuffLen;

	//���ڴ��ı����ͣ�������ʱ�Ļ�����
	char			m_szLargeBuffer[9*1024];
};

}

#endif


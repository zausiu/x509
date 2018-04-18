/***********************************************************************
 * File : CLogNetDriver.h
 * Brief: 定义网络日志的实现类
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
 * 写网络日志的实现类
 */
class CLogNetDriver: public CLogDriver
{
public:
	CLogNetDriver();
	virtual ~CLogNetDriver();

	/**
	 * 设置文件日志的配置
	 * 
	 * @param config 详细的配置参数
	 * 
	 * @return 返回操作是否成功
	 * 		true - 成功
	 * 		false - 失败
	 */
	bool   SetConfig(const CNetLogConfig &config);
		
	/**
	 * 定义写日志接口
	 *
	 * @param [in]	sUrl  url字符串
	 *
	 * @return 操作是否成功
	 *		true - 成功
	 *		false - 失败
	 * 
	 */
	virtual bool Write(const char* szLogInfo, const char*szPrefix, const char* szAppend);	
	
	/**
	 * 对象是否准备写操作
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
	 * 文件驱动对象的pool
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

	//对于大文本发送，采用临时的缓冲区
	char			m_szLargeBuffer[9*1024];
};

}

#endif


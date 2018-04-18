/***********************************************************************
 * File : CLogFileDriver.h
 * Brief: �����ļ���־��ʵ����
 * 
 * History
 * ----------------------------------
 * 2008-11-11	arrowgu   1.0	created
 * 
 ***********************************************************************
 */


#ifndef CLOGFILEDRIVER_H_
#define CLOGFILEDRIVER_H_

#include "CLog.h"
#include "CLogDriver.h"
#include "oi_log2.h"

using std::string;
using namespace SEC::LOG;

namespace SEC
{

/**
 * д�ļ���־��ʵ����
 */
class CLogFileDriver: public CLogDriver
{
public:
	CLogFileDriver();
	virtual ~CLogFileDriver();

	/**
	 * �����ļ���־������
	 * 
	 * @param config ��ϸ�����ò���
	 * 
	 * @return ���ز����Ƿ�ɹ�
	 * 		true - �ɹ�
	 * 		false - ʧ��
	 */
	bool   SetConfig(const CFileLogConfig &config);	
	
	const string& GetFilePath(){ return m_config.GetFilePath(); }
	
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
	virtual bool Write(const char *szLogInfo, const char*szPrefix, const char* szAppend);	
	virtual bool IsReady();

public:
	static CLogDriver* AllocDriver(const CFileLogConfig &config);

private:	
	char* GetLogFileName(const CFileLogConfig &config);	
	bool  InitLogModule(const char* szFilePath);

private:
	LogFile			m_stLogFile;
	char			m_szCurrLogFilePath[256];
	
	CFileLogConfig 	m_config;
	bool			m_bInit;

	char   			m_szFileFullPath[256];

private:
	enum {MAX_LOG_FILE_DRIVERS = 100};
	
	/**
	 * �ļ����������pool
	 */
	static CLogFileDriver  g_aLogFileDrivers[MAX_LOG_FILE_DRIVERS];
	static int			   g_iFileDriverCnt;

	
	
};

}

#endif


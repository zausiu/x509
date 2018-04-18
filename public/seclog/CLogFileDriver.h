/***********************************************************************
 * File : CLogFileDriver.h
 * Brief: 定义文件日志的实现类
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
 * 写文件日志的实现类
 */
class CLogFileDriver: public CLogDriver
{
public:
	CLogFileDriver();
	virtual ~CLogFileDriver();

	/**
	 * 设置文件日志的配置
	 * 
	 * @param config 详细的配置参数
	 * 
	 * @return 返回操作是否成功
	 * 		true - 成功
	 * 		false - 失败
	 */
	bool   SetConfig(const CFileLogConfig &config);	
	
	const string& GetFilePath(){ return m_config.GetFilePath(); }
	
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
	 * 文件驱动对象的pool
	 */
	static CLogFileDriver  g_aLogFileDrivers[MAX_LOG_FILE_DRIVERS];
	static int			   g_iFileDriverCnt;

	
	
};

}

#endif


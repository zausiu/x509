/***********************************************************************
 * File : CLog.h
 * Brief: 定义日志实现类
 * 
 * History
 * ----------------------------------
 * 2008-11-11	arrowgu   1.0	created
 * 
 ***********************************************************************
 */


#ifndef CLOGMGR_H_
#define CLOGMGR_H_

#include <string>
#include <stdint.h>

namespace SEC
{
using std::string;

/**
* 定义调试日志级别, 优先级由高到低
*/
/// Trace级别，记录程序的执行信息
const uint32_t LOG_LEVEL_TRACE	 = 0x01;
/// DEBUG级别，记录业务调试信息
const uint32_t LOG_LEVEL_DBG	 = 0x02;
/// INFO信息,  记录下常用的信息
const uint32_t LOG_LEVEL_INFO	 = 0x04;
/// 警告信息
const uint32_t LOG_LEVEL_WARN	 = 0x08;
/// 错误信息
const uint32_t LOG_LEVEL_ERR	 = 0x10;
/// Assert信息
const uint32_t LOG_LEVEL_ASSERT  = 0x20;
/// 所有的级别
const uint32_t LOG_LEVEL_ALL	 =LOG_LEVEL_TRACE|LOG_LEVEL_DBG
									|LOG_LEVEL_INFO|LOG_LEVEL_WARN
									|LOG_LEVEL_ERR|LOG_LEVEL_ASSERT;

/**
 * 日志模式(文件和NET)
 */
/// 写文件日志
const uint32_t LOG_MODE_FILE =	0x01;
/// 写网络日志
const uint32_t LOG_MODE_NET  = 0x02;
/// 两个日志都写
const uint32_t LOG_MODE_ALL  =	LOG_MODE_FILE|LOG_MODE_NET;

/**
 * 定义日志中的时间格式
 */
enum LogTimeFormat
{
	/// 时间显示到秒
	LOG_TIME_SEC=0,		
	/// 时间显示到毫秒
	LOG_TIME_MSEC
};

/**
 *  定义了文件日志的配置属性
 */
class CFileLogConfig
{
public:	
	/**
	 * 定义日志分组的方式
	 */
	enum ClusterType
	{
		/// 按天分组
		CLUSTER_DAY = 0,
		/// 按小时分组
		CLUSTER_HOUR,
		/// 不分组
		CLUSTER_NONE
	};

	/**
	 * 构造函数，设置属性的初始化值
	 */
	CFileLogConfig();

	/**
	 *  设置文件分组的方式，默认为CLUSTER_DAY
	 */
	bool SetClusterType  (ClusterType emClusterType);

	/**
	 *  设置日志的时间格式，默认为LOG_TIME_SEC
	 */
	bool SetLogTimeFormat(LogTimeFormat emTimeFomat);

	/**
	 * 设置日志文件的路径
 	 */
	bool SetFilePath (const string &sFilePath);

	/**
	 * 设置单个日志文件的最大大小,字节为单位
	 */
	bool SetMaxFileSize(int iMaxFileSize);

	/**
	 * 设置每个cluster最大的日志文件数目
	 */
	bool SetMaxFileCnt(int iMaxFileCnt);

	/**
	 * 设置是否每条日志强制刷新到磁盘，默认为false
	 */
	bool SetFlushMode(bool bForceFlush);

	/**
	 * 获取文件分组的方式
	 */
	ClusterType GetClusterType()const{ return m_emClusterType; }

	/**
	 * 获取日志时间格式 
	 */
	LogTimeFormat  GetLogTimeFormat()const { return m_emLogTimeFormat; }

	/**
	 * 获取日志文件的路径
	 */
	const string& GetFilePath() const {return m_sFilePath; }

	/**
	 * 获取最大文件大小
	 */
	int	GetMaxFileSize()const { return m_iMaxFileSize; }

	/**
	 * 获取没个cluster最大允许的日志文件数目
	 */
	int GetMaxFileCnt()const { return m_iMaxFileCnt; }

	/**
	 * 判断是否需要强制刷新磁盘
	 */
	bool IsForceFlushMode() { return m_bForceFlush; }
	
	
private:
	/// 文件分组的方式，具体取值参考enum ClusterType;默认为CLUSTER_DAY
	ClusterType		m_emClusterType;
	/// 日志中时间的格式，具体取值参考enum TimeFormat;默认为LOG_TIME_SEC
	LogTimeFormat	m_emLogTimeFormat;	
	/// 日志文件路径
	string 			m_sFilePath;
	/// 是否需要每条日志强制刷磁盘
	bool			m_bForceFlush;

	/**
	 * 定义一个cluster下的日志文件限制
	 */
	int			m_iMaxFileSize;	///单个文件的最大大小
	int			m_iMaxFileCnt;	///允许最多的日志文件个数
};

/**
 * 定义了网络日志的配置属性
 */
class CNetLogConfig
{
public:
	CNetLogConfig();

	/**
	 *  设置日志的时间格式，默认为LOG_TIME_SEC
	 */
	bool SetLogTimeFormat(LogTimeFormat emTimeFomat);
	
	/**
	 * 设置本地地址
	 * 
	 * @return 返回操作是否设置成功
	 */
	bool SetLocalAddr(const string& sIp, uint16_t wPort);
	/**
	 * 设置日志服务器地址
	 * 
	 * @return 返回操作是否设置成功
	 */
	bool SetServerAddr(const string &sIp, uint16_t wPort);
	/**
	 * 设置缓冲区大小
	 * 
	 * @param [in] dwSize 字节为单位，最大不允许超过4K,当超过时候，自动截取为4k大小
	 * 
	 * @return 返回操作是否设置成功
	 * 
	 * @note 当程序写入的日志超过缓冲区大小的时候，才发送一个网络包 \n
	 * 	如果不希望缓冲，将dwSize设为0即可
	 */
	bool SetBufferSize(uint32_t dwSize);

	/**
	 * 获取日志时间格式 
	 */
	LogTimeFormat GetLogTimeFormat()const { return m_emLogTimeFormat; }
	/**
	 * 获取本地地址IP
	 */
	const string& GetLocalIp() 	 const { return m_sLocalIp; }
	/**
	 * 获取本地地址端口
	 */
	uint16_t 	  GetLocalPort() const { return m_wLocalPort; }
	/**
	 * 获取服务器IP
	 */
	const string& GetServerIp()  const { return m_sSvrIp; }
	/**
	 * 获取服务器端口
	 */
	uint16_t 	  GetServerPort()const { return m_wSvrPort; }
	/**
	 * 获取日志缓冲区大小
	 */
	uint32_t 	  GetBufferSize()const { return m_dwBufferSize; }
	
private:
	string		m_sLocalIp;
	uint16_t 	m_wLocalPort;
	
	string 		m_sSvrIp;
	uint16_t 	m_wSvrPort;

	uint32_t	m_dwBufferSize;

	
	/// 日志中时间的格式，具体取值参考enum TimeFormat;默认为LOG_TIME_SEC
	LogTimeFormat	m_emLogTimeFormat;	
};

///声明调试等级类
class CLogLevel;

/**
 * 负责所有Log相关的操作
 */
class CLog
{
public:	
	/// 最多日志等级数目
	enum{MAX_LOG_LEVEL_CNT = 32};
	
	CLog();
	virtual ~CLog();

	/**
	 * 设置文件日志的配置属性
	 *
	 * @param [in] dwLevel 要设置属性的日志级别的集合,为LOG_LEVEL_xx的按位或的值
	 * @param [in] config  配置属性
	 *
	 * @return 返回操作是否成功
	 * @note 如果所有的日志级别属性配置一样，则dwLevel = LOG_LEVEL_ALL
	 */
	bool SetFileLogConfig(uint32_t dwLevelSet, const CFileLogConfig &config);

	/**
	 * 设置网络日志的配置属性
	 *
	 * @param [in] dwLevel 要设置属性的日志级别的集合,为LOG_LEVEL_xx的按位或的值
	 * @param [in] config  配置属性
	 *
	 * @return 返回操作是否成功	 
	 * @note 如果所有的日志级别属性配置一样，可设置dwLevel = LOG_LEVEL_ALL
	 */
	bool SetNetLogConfig (uint32_t dwLevelSet, const CNetLogConfig &config);

	/**
	 * 设置当前打印日志级别
	 *
	 * @param [in] dwLevelSet 需要打印的日志级别的集合，为LOG_LEVEL_xx的按位或的值
	 *
	 * @return 返回操作是否成功
	 */
	bool SetLogLevel(uint32_t dwLevelSet);

	/**
	 * 设置当前打印日志模式
	 *
	 * @param [in] dwMode 日志模式，
	 *			LOG_MODE_FILE - 只记录文件日志
	 *			或LOG_MODE_NET或LOG_MODE_FILE|LOG_MODE_NET
	 *
	 * @return 返回操作是否成功
	 */
	bool SetLogMode(uint32_t dwMode);

	/**
	 * 判断是否需要记录日志
	 * @note 记录日志的操作一般都有log.h定义的宏来封装调用
	 */
	bool IsNeedLog(uint32_t dwLevel);
	
	/**
	 * 记录日志信息
	 *
	 * @return 返回是否成功
	 * @note 调用该函数之前一定要先调用IsNeedLog判断下是否需要记录日志
	 * @note 这儿将原来的函数接口Log(...)可变参数的接口删除,是为了防止一些基础库没有同时编译的情况，
	 *	这儿强制所有依赖的库要重新编译
	 *
	 * @note 记录日志的操作一般都有log.h定义的宏来封装调用
	 */
	bool Log(uint32_t dwLevel, const char *szLogInfo);

	/**
	 * 设置日志视图属性
	 *
	 * @param [in] dwLevelSet 需要记录视图属性的级别
	 * @parma [in] iMntAttr 视图属性值
	 * 
	 */
	void SetMntAttr(uint32_t dwLevelSet, int iMntAttr);

	/**
	 * 设置日志内容的前缀
	 *
	 * @note 此接口很少使用，建议使用SetAppendInfo代替
	 */
	void SetPrefixInfo(const char* szStr);

	/**
	 * 设置日志内容的后缀
	 * @note 同上
	 */
	void SetPrefixInfo(const string& sStr);
	
	/**
	 * 设置日志内容的后缀内容
	 */
	void SetAppendInfo(const char* szStr);

	/**
	 * 设置日志内容的后缀内容
	 *
	 * @note 为了兼容老String版本
	 */
	void SetAppendInfo(const string& sStr);

    void ClearAppendInfo();

public:
	/**
	 * 单例模式
	 */
	static CLog* Instance();

private:
	int  iMapLevel2Idx(uint32_t dwLevel);
	bool iOutput(CLogLevel *pLevel, const char* sLogInfo);

private:
	uint32_t	m_dwMode;
	uint32_t	m_dwLevel;

	CLogLevel * m_aLevels;
	
	char		m_szAppendInfo[4096];
	char		m_szPrefixInfo[4096];
};

}

#endif


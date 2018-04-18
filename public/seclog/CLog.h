/***********************************************************************
 * File : CLog.h
 * Brief: ������־ʵ����
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
* ���������־����, ���ȼ��ɸߵ���
*/
/// Trace���𣬼�¼�����ִ����Ϣ
const uint32_t LOG_LEVEL_TRACE	 = 0x01;
/// DEBUG���𣬼�¼ҵ�������Ϣ
const uint32_t LOG_LEVEL_DBG	 = 0x02;
/// INFO��Ϣ,  ��¼�³��õ���Ϣ
const uint32_t LOG_LEVEL_INFO	 = 0x04;
/// ������Ϣ
const uint32_t LOG_LEVEL_WARN	 = 0x08;
/// ������Ϣ
const uint32_t LOG_LEVEL_ERR	 = 0x10;
/// Assert��Ϣ
const uint32_t LOG_LEVEL_ASSERT  = 0x20;
/// ���еļ���
const uint32_t LOG_LEVEL_ALL	 =LOG_LEVEL_TRACE|LOG_LEVEL_DBG
									|LOG_LEVEL_INFO|LOG_LEVEL_WARN
									|LOG_LEVEL_ERR|LOG_LEVEL_ASSERT;

/**
 * ��־ģʽ(�ļ���NET)
 */
/// д�ļ���־
const uint32_t LOG_MODE_FILE =	0x01;
/// д������־
const uint32_t LOG_MODE_NET  = 0x02;
/// ������־��д
const uint32_t LOG_MODE_ALL  =	LOG_MODE_FILE|LOG_MODE_NET;

/**
 * ������־�е�ʱ���ʽ
 */
enum LogTimeFormat
{
	/// ʱ����ʾ����
	LOG_TIME_SEC=0,		
	/// ʱ����ʾ������
	LOG_TIME_MSEC
};

/**
 *  �������ļ���־����������
 */
class CFileLogConfig
{
public:	
	/**
	 * ������־����ķ�ʽ
	 */
	enum ClusterType
	{
		/// �������
		CLUSTER_DAY = 0,
		/// ��Сʱ����
		CLUSTER_HOUR,
		/// ������
		CLUSTER_NONE
	};

	/**
	 * ���캯�����������Եĳ�ʼ��ֵ
	 */
	CFileLogConfig();

	/**
	 *  �����ļ�����ķ�ʽ��Ĭ��ΪCLUSTER_DAY
	 */
	bool SetClusterType  (ClusterType emClusterType);

	/**
	 *  ������־��ʱ���ʽ��Ĭ��ΪLOG_TIME_SEC
	 */
	bool SetLogTimeFormat(LogTimeFormat emTimeFomat);

	/**
	 * ������־�ļ���·��
 	 */
	bool SetFilePath (const string &sFilePath);

	/**
	 * ���õ�����־�ļ�������С,�ֽ�Ϊ��λ
	 */
	bool SetMaxFileSize(int iMaxFileSize);

	/**
	 * ����ÿ��cluster������־�ļ���Ŀ
	 */
	bool SetMaxFileCnt(int iMaxFileCnt);

	/**
	 * �����Ƿ�ÿ����־ǿ��ˢ�µ����̣�Ĭ��Ϊfalse
	 */
	bool SetFlushMode(bool bForceFlush);

	/**
	 * ��ȡ�ļ�����ķ�ʽ
	 */
	ClusterType GetClusterType()const{ return m_emClusterType; }

	/**
	 * ��ȡ��־ʱ���ʽ 
	 */
	LogTimeFormat  GetLogTimeFormat()const { return m_emLogTimeFormat; }

	/**
	 * ��ȡ��־�ļ���·��
	 */
	const string& GetFilePath() const {return m_sFilePath; }

	/**
	 * ��ȡ����ļ���С
	 */
	int	GetMaxFileSize()const { return m_iMaxFileSize; }

	/**
	 * ��ȡû��cluster����������־�ļ���Ŀ
	 */
	int GetMaxFileCnt()const { return m_iMaxFileCnt; }

	/**
	 * �ж��Ƿ���Ҫǿ��ˢ�´���
	 */
	bool IsForceFlushMode() { return m_bForceFlush; }
	
	
private:
	/// �ļ�����ķ�ʽ������ȡֵ�ο�enum ClusterType;Ĭ��ΪCLUSTER_DAY
	ClusterType		m_emClusterType;
	/// ��־��ʱ��ĸ�ʽ������ȡֵ�ο�enum TimeFormat;Ĭ��ΪLOG_TIME_SEC
	LogTimeFormat	m_emLogTimeFormat;	
	/// ��־�ļ�·��
	string 			m_sFilePath;
	/// �Ƿ���Ҫÿ����־ǿ��ˢ����
	bool			m_bForceFlush;

	/**
	 * ����һ��cluster�µ���־�ļ�����
	 */
	int			m_iMaxFileSize;	///�����ļ�������С
	int			m_iMaxFileCnt;	///����������־�ļ�����
};

/**
 * ������������־����������
 */
class CNetLogConfig
{
public:
	CNetLogConfig();

	/**
	 *  ������־��ʱ���ʽ��Ĭ��ΪLOG_TIME_SEC
	 */
	bool SetLogTimeFormat(LogTimeFormat emTimeFomat);
	
	/**
	 * ���ñ��ص�ַ
	 * 
	 * @return ���ز����Ƿ����óɹ�
	 */
	bool SetLocalAddr(const string& sIp, uint16_t wPort);
	/**
	 * ������־��������ַ
	 * 
	 * @return ���ز����Ƿ����óɹ�
	 */
	bool SetServerAddr(const string &sIp, uint16_t wPort);
	/**
	 * ���û�������С
	 * 
	 * @param [in] dwSize �ֽ�Ϊ��λ�����������4K,������ʱ���Զ���ȡΪ4k��С
	 * 
	 * @return ���ز����Ƿ����óɹ�
	 * 
	 * @note ������д�����־������������С��ʱ�򣬲ŷ���һ������� \n
	 * 	�����ϣ�����壬��dwSize��Ϊ0����
	 */
	bool SetBufferSize(uint32_t dwSize);

	/**
	 * ��ȡ��־ʱ���ʽ 
	 */
	LogTimeFormat GetLogTimeFormat()const { return m_emLogTimeFormat; }
	/**
	 * ��ȡ���ص�ַIP
	 */
	const string& GetLocalIp() 	 const { return m_sLocalIp; }
	/**
	 * ��ȡ���ص�ַ�˿�
	 */
	uint16_t 	  GetLocalPort() const { return m_wLocalPort; }
	/**
	 * ��ȡ������IP
	 */
	const string& GetServerIp()  const { return m_sSvrIp; }
	/**
	 * ��ȡ�������˿�
	 */
	uint16_t 	  GetServerPort()const { return m_wSvrPort; }
	/**
	 * ��ȡ��־��������С
	 */
	uint32_t 	  GetBufferSize()const { return m_dwBufferSize; }
	
private:
	string		m_sLocalIp;
	uint16_t 	m_wLocalPort;
	
	string 		m_sSvrIp;
	uint16_t 	m_wSvrPort;

	uint32_t	m_dwBufferSize;

	
	/// ��־��ʱ��ĸ�ʽ������ȡֵ�ο�enum TimeFormat;Ĭ��ΪLOG_TIME_SEC
	LogTimeFormat	m_emLogTimeFormat;	
};

///�������Եȼ���
class CLogLevel;

/**
 * ��������Log��صĲ���
 */
class CLog
{
public:	
	/// �����־�ȼ���Ŀ
	enum{MAX_LOG_LEVEL_CNT = 32};
	
	CLog();
	virtual ~CLog();

	/**
	 * �����ļ���־����������
	 *
	 * @param [in] dwLevel Ҫ�������Ե���־����ļ���,ΪLOG_LEVEL_xx�İ�λ���ֵ
	 * @param [in] config  ��������
	 *
	 * @return ���ز����Ƿ�ɹ�
	 * @note ������е���־������������һ������dwLevel = LOG_LEVEL_ALL
	 */
	bool SetFileLogConfig(uint32_t dwLevelSet, const CFileLogConfig &config);

	/**
	 * ����������־����������
	 *
	 * @param [in] dwLevel Ҫ�������Ե���־����ļ���,ΪLOG_LEVEL_xx�İ�λ���ֵ
	 * @param [in] config  ��������
	 *
	 * @return ���ز����Ƿ�ɹ�	 
	 * @note ������е���־������������һ����������dwLevel = LOG_LEVEL_ALL
	 */
	bool SetNetLogConfig (uint32_t dwLevelSet, const CNetLogConfig &config);

	/**
	 * ���õ�ǰ��ӡ��־����
	 *
	 * @param [in] dwLevelSet ��Ҫ��ӡ����־����ļ��ϣ�ΪLOG_LEVEL_xx�İ�λ���ֵ
	 *
	 * @return ���ز����Ƿ�ɹ�
	 */
	bool SetLogLevel(uint32_t dwLevelSet);

	/**
	 * ���õ�ǰ��ӡ��־ģʽ
	 *
	 * @param [in] dwMode ��־ģʽ��
	 *			LOG_MODE_FILE - ֻ��¼�ļ���־
	 *			��LOG_MODE_NET��LOG_MODE_FILE|LOG_MODE_NET
	 *
	 * @return ���ز����Ƿ�ɹ�
	 */
	bool SetLogMode(uint32_t dwMode);

	/**
	 * �ж��Ƿ���Ҫ��¼��־
	 * @note ��¼��־�Ĳ���һ�㶼��log.h����ĺ�����װ����
	 */
	bool IsNeedLog(uint32_t dwLevel);
	
	/**
	 * ��¼��־��Ϣ
	 *
	 * @return �����Ƿ�ɹ�
	 * @note ���øú���֮ǰһ��Ҫ�ȵ���IsNeedLog�ж����Ƿ���Ҫ��¼��־
	 * @note �����ԭ���ĺ����ӿ�Log(...)�ɱ�����Ľӿ�ɾ��,��Ϊ�˷�ֹһЩ������û��ͬʱ����������
	 *	���ǿ�����������Ŀ�Ҫ���±���
	 *
	 * @note ��¼��־�Ĳ���һ�㶼��log.h����ĺ�����װ����
	 */
	bool Log(uint32_t dwLevel, const char *szLogInfo);

	/**
	 * ������־��ͼ����
	 *
	 * @param [in] dwLevelSet ��Ҫ��¼��ͼ���Եļ���
	 * @parma [in] iMntAttr ��ͼ����ֵ
	 * 
	 */
	void SetMntAttr(uint32_t dwLevelSet, int iMntAttr);

	/**
	 * ������־���ݵ�ǰ׺
	 *
	 * @note �˽ӿں���ʹ�ã�����ʹ��SetAppendInfo����
	 */
	void SetPrefixInfo(const char* szStr);

	/**
	 * ������־���ݵĺ�׺
	 * @note ͬ��
	 */
	void SetPrefixInfo(const string& sStr);
	
	/**
	 * ������־���ݵĺ�׺����
	 */
	void SetAppendInfo(const char* szStr);

	/**
	 * ������־���ݵĺ�׺����
	 *
	 * @note Ϊ�˼�����String�汾
	 */
	void SetAppendInfo(const string& sStr);

    void ClearAppendInfo();

public:
	/**
	 * ����ģʽ
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


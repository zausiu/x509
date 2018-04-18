/***********************************************************************
 * File : CLogDriver.h
 * Brief: ��������־ʵ�ֵĽӿ�
 * 
 * History
 * ----------------------------------
 * 2008-11-11	arrowgu   1.0	created
 * 
 ***********************************************************************
 */


#ifndef CLOGDRIVER_H_
#define CLOGDRIVER_H_

#include <string>

namespace SEC
{
using std::string;

/**
 * ��������־��ʵ�ֽӿ�
 */
class CLogDriver
{
public:
	virtual ~CLogDriver(){}
	
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
	virtual bool Write(const char* szLogInfo, const char*szPrefix, const char* szAppend) = 0;

	virtual bool IsReady() = 0;
};

}

#endif


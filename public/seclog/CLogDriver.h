/***********************************************************************
 * File : CLogDriver.h
 * Brief: 定义了日志实现的接口
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
 * 定义了日志的实现接口
 */
class CLogDriver
{
public:
	virtual ~CLogDriver(){}
	
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
	virtual bool Write(const char* szLogInfo, const char*szPrefix, const char* szAppend) = 0;

	virtual bool IsReady() = 0;
};

}

#endif


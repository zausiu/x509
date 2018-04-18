/***********************************************************************
 * File : log.h
 * Brief: ���峣�õ���־��
 * 
 * History
 * ----------------------------------
 * 2008-11-11	arrowgu   1.0	created
 * 
 ***********************************************************************
 */


#ifndef SEC_LOG_H_
#define SEC_LOG_H_

#include <stdint.h>
#include "CLog.h"

/**
 * ������־��İ汾�ţ�ÿ���޸���Ҫ�ۼӰ汾
 */
#define SEC_LOG_LIB_VERSION "1.1.2"

/**
 * �û�ֱ��ʹ�õ���־��
 */
#define LG_TRACE(fmt, args...) 	do{SecLogOutput(SEC::LOG_LEVEL_TRACE, "%s|%s|%d|%s|" fmt, "TRACE", __FILE__, __LINE__, __FUNCTION__ , ##args);}while(0)
#define LG_DBG(fmt, args...) 		do{SecLogOutput(SEC::LOG_LEVEL_DBG,   "%s|%s|%d|%s|" fmt, "DBG",   __FILE__, __LINE__, __FUNCTION__ , ##args);}while(0)
#define LG_INFO(fmt, args...) 		do{SecLogOutput(SEC::LOG_LEVEL_INFO,  "%s|%s|%d|%s|" fmt, "INFO",  __FILE__, __LINE__, __FUNCTION__ , ##args);}while(0)
#define LG_WARN(fmt, args...) 		do{SecLogOutput(SEC::LOG_LEVEL_WARN,  "%s|%s|%d|%s|" fmt, "WARN",  __FILE__, __LINE__, __FUNCTION__ , ##args);}while(0)
#define LG_ERR(fmt, args...) 		do{SecLogOutput(SEC::LOG_LEVEL_ERR, 	"%s|%s|%d|%s|" fmt, "ERR", __FILE__, __LINE__, __FUNCTION__ , ##args);}while(0)
#define LG_ASSERT(fmt, args...) 	do{SecLogOutput(SEC::LOG_LEVEL_ASSERT, 	"%s|%s|%d|%s|" fmt, "ASSERT", __FILE__, __LINE__, __FUNCTION__ , ##args);}while(0)



#ifdef  __cplusplus
extern "C" {
#endif


/**
 * Ϊ�˶��ϲ�Ŀ�����CLog�����ϸ�ڣ�����������Ĵ�ӡ��־���ܵ�������������ӿ�
 */
void SecLogOutput(uint32_t dwLevel, const char *szFormat, ...);


#ifdef  __cplusplus
}
#endif


#endif


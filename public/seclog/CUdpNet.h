/***********************************************************************
 * File : CUdpNet.h
 * Brief: ��װUDPͨѶ�Ŀͻ��˳��ù���
 * 
 * History
 * ----------------------------------
 * 2008-11-11	arrowgu   1.0	created
 * 
 ***********************************************************************
 */

#ifndef CUDPNET_H_
#define CUDPNET_H_

#include <string>
#include "CNet.h"

namespace SEC
{
namespace LOG
{
using std::string;

/**
 * ��װ��UDPͨѶ�Ĳ���
 */
class CUdpNet: public CNet
{
public:
	bool Create(const string &sLocalIp, uint16_t wPort);
	
	int  SendTo(const uint8_t *pBuff, int iLen,
			const string &sDstIp, uint16_t wDstPort);
	
	int  RecvFrom(uint8_t *pBuff, int iLen, int iTimeout);

};

}
}

#endif


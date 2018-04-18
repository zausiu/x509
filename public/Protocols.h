/*
 * Protocols.h
 *
 *  Created on: Mar 3, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef PUBLIC_PROTOCOLS_H_
#define PUBLIC_PROTOCOLS_H_

#include <boost/shared_ptr.hpp>
#include "gleaner.h"
#include "json/json.h"
#include "CommonHelper.h"

#pragma pack(1)

#define MAGIC_CODE 0x12345678
struct GleanerHead
{
	uint32_t magic_code;
	int json_len;
	uint64_t reserved;
};

#define STX 0x02
#define ETX 0x03
struct CgiHead
{
//	uint32_t magic_code;        // magic code, it should and have to be 0x02
	char stx;
	uint32_t total_pkt_len;	    // including the head
	uint32_t client_ip;	// �ͻ���ip
	uint32_t cmd;		// ��ѯ������
	uint32_t seq;		// ������ϵͳ�����ش��ֶ�
	char	 user[32];		//�û�
    char 	 session[64];
    char     ticket[480];	//oaticket
    char     reserved[128];	//Ԥ���ֶΣ�Э����չ��
    char     data[0];        //�䳤data
};
#pragma pack()

#define CGI_HEAD_LEN sizeof(CgiHead)

typedef boost::shared_ptr<CgiHead> SharedCgiPkt;
SharedCgiPkt CreateCgiPacket(CgiHead& reqhead,const char* data, int data_len);
SharedCgiPkt CreateCgiPacket(const char* data, int data_len, const char* user);
SharedSmartMem CreateCgiPacket2(const char* data, int data_len, const char* user);

inline int GetPacketTotalLen(void* buff)
{
	uint32_t len = *(uint32_t*)((char*)buff + 1);
	return (int)len;
}

boost::shared_ptr<AccessRecord> ConvertJson2AccessRecord(const Json::Value& jdoc);
boost::shared_ptr<AccessRecord> ConvertJson2AccessRecordV3(const Json::Value& jdoc);
boost::shared_ptr<InterfaceRecord> ConvertJson2InterfaceRecord(const Json::Value& jdoc);

enum TheQueryType
{
	TQR_SUMMARY = 10001,

	TQR_SYSTEM_BRIEF = 20001, // Deprecated
	TQR_EDIT_SYSTEM_BRIEF = 20005,

	TQR_SYSTEM_BRIEF2 = 20010,
	TQR_LIST_ALLSYSNAMES2 = 20011,
	TQR_EDIT_SYSTEM_BRIEF2 = 20015,
	TAR_DEL_EVENT_SYSTEM_BRIEF2 = 20016,
	TQR_LIST_ENUMS_SYSTEM_BRIEF2 = 20020,
	TQR_ADD_ENUMS_SYSTEM_BRIEF2 = 20022,
	TQR_RENAME_ENUMS_SYSTEM_BRIEF2 = 20023,
	TQR_DEL_ENUMS_SYSTEM_BRIEF2 = 20024,

	TQR_LIST_ACCESSLOGS = 30001,
	TQR_LIST_ACCESSLOGS_NEO = 30003,
	TQR_GET_ACCESSLOG_NEO = 30004,
	TQR_LIST_INTERFACELOGS = 30005,
	TQR_SEARCH_ACCESSLOG_NEO = 30010,

	TQR_TRENDS = 40001,

	TQR_AUDIT = 50001,

	TQR_HOTGUYS = 51001,
	TQR_QUOTA_SYSTEMS = 51005,

	TQR_LIST_WARN = 60001,
	TQR_WARN_STATUS = 60002,
	TQR_EDIT_WARN = 60005,

	TQR_GH_FILTER_SYSTEM_BRIEF = 90001,

	/////////////////////////
	TQR_JOB_HOTGUYS = 1000000,  // 预准备
};

enum TheMessageType
{
	TMSG_WE_NAME2UIN = 100000,
	TMSG_CUTE_QQBASICINFO = 101000,
	TMSG_CUTE_WEBASICINFO = 102000,
};

void InitTheXTypeDescs();
const std::string GetTheQueryTypeDesc(TheQueryType qt);
const std::string GetTheMessageTypeDesc(TheMessageType qt);

#endif /* PUBLIC_PROTOCOLS_H_ */

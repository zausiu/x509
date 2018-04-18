/*
 * Protocols.h
 *
 *  Created on: Mar 3, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wblog.ykyi.net
 */
#include <iostream>
#include <string.h>
#include <boost/make_shared.hpp>
#include <boost/unordered_map.hpp>
#include "Protocols.h"

boost::unordered_map<TheQueryType, std::string> g_query_type_descs;
boost::unordered_map<TheMessageType, std::string> g_msg_type_descs;

#define RetrieveJsonElem(holder, jdoc, kname, tran) do{ const Json::Value& jv__ = jdoc[kname]; \
														if (!jv__.isNull()){   \
	                                                       holder = jv__.tran(); } \
													}while(false)
#define RetrieveJsonElem_cast(holder, jdoc, kname, tran, cast) do{ const Json::Value& jv__ = jdoc[kname]; \
														if (!jv__.isNull()){   \
	                                                       holder = static_cast<cast>(jv__.tran()); } \
													}while(false)

boost::shared_ptr<AccessRecord> ConvertJson2AccessRecord(const Json::Value& jdoc)
{
	boost::shared_ptr<AccessRecord> record = boost::make_shared<AccessRecord>();

	RetrieveJsonElem_cast(record->sys_id, jdoc, "sys_id", asInt, SystemID);
	RetrieveJsonElem(record->sys_ip, jdoc, "sys_ip", asString);
	RetrieveJsonElem(record->user_name, jdoc, "user_name", asString);
	RetrieveJsonElem_cast(record->user_org, jdoc, "user_org", asInt, Organization);
	RetrieveJsonElem(record->user_ip, jdoc, "user_ip", asString);
	RetrieveJsonElem(record->req_cmd, jdoc, "req_cmd", asString);
	RetrieveJsonElem(record->req_content, jdoc, "req_content", asString);
	RetrieveJsonElem(record->req_time, jdoc, "req_time", asString);
	RetrieveJsonElem(record->resp_content, jdoc, "resp_content", asString);
	RetrieveJsonElem(record->resp_time, jdoc, "resp_time", asString);
	RetrieveJsonElem_cast(record->param_type, jdoc, "param_type", asInt, ParamType);
	RetrieveJsonElem(record->param, jdoc, "param", asString);
	RetrieveJsonElem_cast(record->result, jdoc, "result", asInt, Result);
	RetrieveJsonElem(record->is_subreq, jdoc, "is_subreq", asBool);
	RetrieveJsonElem_cast(record->super_req_type, jdoc, "super_req_type", asInt, ParamType);
	RetrieveJsonElem(record->super_param, jdoc, "super_param", asString);
	RetrieveJsonElem(record->magic, jdoc, "magic", asString);
	RetrieveJsonElem(record->misc0, jdoc, "misc", asString);
	RetrieveJsonElem(record->misc0, jdoc, "misc0", asString);
	RetrieveJsonElem(record->misc1, jdoc, "misc1", asString);

	return record;
}

boost::shared_ptr<AccessRecord> ConvertJson2AccessRecordV3(const Json::Value& jdoc)
{
	boost::shared_ptr<AccessRecord> record = boost::make_shared<AccessRecord>();
	record->retcode = NOTSET_RETCODE;

	RetrieveJsonElem_cast(record->sys_id, jdoc, "sys_id", asInt, SystemID);
	RetrieveJsonElem(record->sys_ip, jdoc, "sys_ip", asString);
	RetrieveJsonElem(record->user_name, jdoc, "user_name", asString);
	RetrieveJsonElem_cast(record->user_org, jdoc, "user_org", asInt, Organization);
	RetrieveJsonElem(record->user_ip, jdoc, "user_ip", asString);
	RetrieveJsonElem(record->req_cmd, jdoc, "req_cmd", asString);
	RetrieveJsonElem(record->req_content, jdoc, "req_content", asString);
	RetrieveJsonElem(record->req_time, jdoc, "req_time", asString);
	RetrieveJsonElem(record->resp_content, jdoc, "resp_content", asString);
	RetrieveJsonElem(record->resp_time, jdoc, "resp_time", asString);
	RetrieveJsonElem_cast(record->param_type, jdoc, "param_type", asInt, ParamType);
	RetrieveJsonElem(record->param, jdoc, "param", asString);
	RetrieveJsonElem_cast(record->result, jdoc, "result", asInt, Result);
	RetrieveJsonElem(record->retcode, jdoc, "retcode", asInt);
	RetrieveJsonElem(record->is_subreq, jdoc, "is_subreq", asBool);
	RetrieveJsonElem_cast(record->super_req_type, jdoc, "super_req_type", asInt, ParamType);
	RetrieveJsonElem(record->super_param, jdoc, "super_param", asString);
	RetrieveJsonElem(record->misc0, jdoc, "misc0", asString);
	RetrieveJsonElem(record->misc1, jdoc, "misc1", asString);

	return record;
}

boost::shared_ptr<InterfaceRecord> ConvertJson2InterfaceRecord(const Json::Value& jdoc)
{
	boost::shared_ptr<InterfaceRecord> record = boost::make_shared<InterfaceRecord>();

	RetrieveJsonElem(record->ifid, jdoc, "ifid", asInt);
	RetrieveJsonElem_cast(record->owner_sys_id, jdoc, "owner_sys_id", asInt, SystemID);
	RetrieveJsonElem(record->owner_sys_ip, jdoc, "TQR_SYSTEM_LIST_ALLSYSNAMESowner_sys_ip", asString);

	RetrieveJsonElem_cast(record->req_sys_id, jdoc, "req_sys_id", asInt, SystemID);
	RetrieveJsonElem(record->req_sys_ip, jdoc, "req_sys_ip", asString);
	RetrieveJsonElem(record->user, jdoc, "user", asString);

	RetrieveJsonElem_cast(record->param_type, jdoc, "param_type", asInt, ParamType);
	RetrieveJsonElem(record->param, jdoc, "param", asString);

	RetrieveJsonElem(record->req_time, jdoc, "req_time", asString);
	RetrieveJsonElem(record->req_content, jdoc, "req_content", asString);
	RetrieveJsonElem(record->deliberate_req, jdoc, "deliberate_req", asBool);

	RetrieveJsonElem(record->resp_time, jdoc, "resp_time", asString);
	RetrieveJsonElem(record->resp_content, jdoc, "resp_content", asString);
	RetrieveJsonElem_cast(record->result, jdoc, "result", asInt, Result);

	RetrieveJsonElem(record->misc, jdoc, "misc", asString);

	return record;
}

SharedCgiPkt CreateCgiPacket(CgiHead& reqhead,const char* data, int data_len)
{
	uint32_t total_len = sizeof(CgiHead) + data_len + 1;
	char* mem = new char[total_len];
	CgiHead* head = reinterpret_cast<CgiHead*>(mem);

	memcpy(head, &reqhead, sizeof(reqhead));
	memcpy(head->data, data, data_len);

	head->stx = STX;
	head->total_pkt_len = total_len;
	mem[total_len - 1] = ETX;

	boost::shared_ptr<CgiHead> pkt;
	pkt.reset(head);

	return pkt;
}

SharedCgiPkt CreateCgiPacket(const char* data, int data_len, const char* user)
{
	uint32_t total_len = sizeof(CgiHead) + data_len + 1;
	char* mem = new char[total_len];
	CgiHead* head = reinterpret_cast<CgiHead*>(mem);

	memcpy(head->data, data, data_len);
	head->stx = STX;
	strncpy(head->user, user, sizeof(head->user));
	head->total_pkt_len = total_len;
	mem[total_len - 1] = ETX;

	boost::shared_ptr<CgiHead> pkt;
	pkt.reset(head);

	return pkt;
}

SharedSmartMem CreateCgiPacket2(const char* data, int data_len, const char* user)
{
	uint32_t total_len = sizeof(CgiHead) + data_len + 1;
	SharedSmartMem mem = boost::make_shared<SmartMem>(total_len);
	CgiHead* head = reinterpret_cast<CgiHead*>(mem->data());

	memcpy(head->data, data, data_len);
	head->stx = STX;
	strncpy(head->user, user, sizeof(head->user));
	head->total_pkt_len = total_len;
	mem->data()[total_len - 1] = ETX;

	return mem;
}

void InitTheXTypeDescs()
{
	g_query_type_descs[TQR_SUMMARY] = "summary";

	g_query_type_descs[TQR_SYSTEM_BRIEF] = "system_brief";
	g_query_type_descs[TQR_EDIT_SYSTEM_BRIEF] = "edit_system_brief";

	g_query_type_descs[TQR_SYSTEM_BRIEF2] = "system_brief2";
	g_query_type_descs[TQR_LIST_ALLSYSNAMES2] = "list_allsysnames2";
    g_query_type_descs[TQR_EDIT_SYSTEM_BRIEF2] = "edit_system_brief2";
    g_query_type_descs[TAR_DEL_EVENT_SYSTEM_BRIEF2] = "del_event_system_brief2";
	g_query_type_descs[TQR_LIST_ENUMS_SYSTEM_BRIEF2] = "list_enums_system_brief2";
	g_query_type_descs[TQR_ADD_ENUMS_SYSTEM_BRIEF2] = "add_enums_system_brief2";
	g_query_type_descs[TQR_RENAME_ENUMS_SYSTEM_BRIEF2] = "rename_enums_system_brief2";
	g_query_type_descs[TQR_DEL_ENUMS_SYSTEM_BRIEF2] = "del_enums_system_brief2";
	g_query_type_descs[TQR_GH_FILTER_SYSTEM_BRIEF] = "gh_filter_system_brief";

	g_query_type_descs[TQR_LIST_ACCESSLOGS] = "list_accesslogs";
	g_query_type_descs[TQR_LIST_ACCESSLOGS_NEO] = "list_accesslogs_neo";
	g_query_type_descs[TQR_SEARCH_ACCESSLOG_NEO] = "search_accesslog_neo";
	g_query_type_descs[TQR_GET_ACCESSLOG_NEO] = "get_accesslog_neo";
	g_query_type_descs[TQR_LIST_INTERFACELOGS] = "list_interfacelogs";
	g_query_type_descs[TQR_TRENDS] = "list_interfacelogs";
	g_query_type_descs[TQR_AUDIT] = "audit";
	g_query_type_descs[TQR_LIST_WARN] = "list_warn";
	g_query_type_descs[TQR_WARN_STATUS] = "warn_status";
	g_query_type_descs[TQR_EDIT_WARN] = "edit_warn";

	g_query_type_descs[TQR_JOB_HOTGUYS] = "HotGuy_JOB";
	g_query_type_descs[TQR_HOTGUYS] = "HotGuys";

	g_query_type_descs[TQR_QUOTA_SYSTEMS] = "QuotaSystems";

	g_msg_type_descs[TMSG_WE_NAME2UIN] = "we_name2uin";
	g_msg_type_descs[TMSG_CUTE_QQBASICINFO] = "qq_basicinfo";
	g_msg_type_descs[TMSG_CUTE_WEBASICINFO] = "we_basicinfo";
}

const std::string GetTheQueryTypeDesc(TheQueryType qt)
{
	static const std::string stranger("UNRECOGNIZED_QUERYTYPE");

	try
	{
		const std::string& desc = g_query_type_descs.at(qt);
		return desc;
	}
	catch (std::out_of_range& ex)
	{
		std::cerr << ex.what() << " QueryType: " << qt << "\n";
		return stranger;
	}
}

const std::string GetTheMessageTypeDesc(TheMessageType mt)
{
	static const std::string stranger("UNRECOGNIZED_MESSAGETYPE");

	try
	{
		const std::string& desc = g_msg_type_descs.at(mt);
		return desc;
	}
	catch (std::out_of_range& ex)
	{
		std::cerr << ex.what() << " QueryType: " << mt << "\n";
		return stranger;
	}
}

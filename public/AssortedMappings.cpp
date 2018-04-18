/*
 * AssortedMappings.cpp
 *
 *  Created on: Mar 18, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wblog.ykyi.net 
 */
#include <set>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include "AssortedMappings.h"
#include "CommonHelper.h"

std::string UNKNOWN("UNKNOWN");
BiMapper<SystemID, std::string> system_id_map;
BiMapper<Organization, std::string> organization_map;
boost::unordered_map<Organization, std::string> org2std_map;
std::set<std::string> std_org_set;
BiMapper<ParamType, std::string> paramtype_map;
BiMapper<Result, std::string> result_map;

void InitAssortedMappings();

RunOnce(InitAssortedMappings(), _);

void InitAssortedMappings()
{
	system_id_map.Insert(SYS_ZHONGJIANG, "ZHONGJIANG");
	system_id_map.Insert(SYS_531, "531");
	system_id_map.Insert(SYS_FC, "FC");
	system_id_map.Insert(SYS_PAIHAO, "PAIHAO");
	system_id_map.Insert(SYS_PUSH_DATA, "PUSH_DATA");
	system_id_map.Insert(SYS_TONGDA, "TONGDA");
	system_id_map.Insert(SYS_OLD_XINDA, "OLD_XINDA");
	system_id_map.Insert(SYS_XINDA, "XINDA");
	system_id_map.Insert(SYS_GUOJIA_QQ, "GUOJIA_QQ");
	system_id_map.Insert(SYS_GUOJIA_WX, "GUOJIA_WX");
	system_id_map.Insert(SYS_ANTI_TERRORISM, "ANTI_TERRORISM");
	system_id_map.Insert(SYS_ZHONGJIANG_INVOICE, "ZHONGJIANG_INVOICE");
	system_id_map.Insert(SYS_ZHONGJIANG_ACCOUNT, "ZHONGJIANG_ACCOUNT");
	system_id_map.Insert(SYS_DIAOZHENG, "DIAOZHENG");
	system_id_map.Insert(SYS_630_PO, "630_PO");
	system_id_map.Insert(SYS_PUSH_WEGH, "PUSH_WEGH");
	system_id_map.Insert(SYS_WB, "WB");
	system_id_map.Insert(SYS_WB2, "WB2");
	system_id_map.Insert(SYS_WEGH_ARTICLES, "WEGH_ARTICLES");
	system_id_map.Insert(SYS_CREDIT, "CREDIT");
	system_id_map.Insert(SYS_630, "630");
	system_id_map.Insert(SYS_LBS, "LBS");
	system_id_map.Insert(SYS_BIG, "BIG");
	system_id_map.Insert(SYS_SPORE, "SPORE");
	system_id_map.Insert(SYS_INTERNAL_PAIHAO, "INTERNAL_PAIHAO");
	system_id_map.Insert(SYS_EXPRESS, "EXPRESS");
	system_id_map.Insert(SYS_EMERGENCYGRP_WEIDC, "EMERGENCYGRP_WEIDC");
	system_id_map.Insert(SYS_QQGROUP_BACKTRACE, "QQGROUP_BACKTRACE");
	system_id_map.Insert(SYS_WEGROUP_BACKTRACE, "WEGROUP_BACKTRACE");
	system_id_map.Insert(SYS_XJJS, "XJJS");
	system_id_map.Insert(SYS_531FOR_XIAOJIA, "531FOR_XIAOJIA");
	system_id_map.Insert(SYS_531FOR_11B, "531FOR_11B");
	system_id_map.Insert(SYS_531FOR_12B, "531FOR_12B");
	system_id_map.Insert(SYS_531FOR_XJ_ZHONGJIANG, "531FOR_XJ_ZHONGJIANG");
	system_id_map.Insert(SYS_531FOR_XJ, "531FOR_XJ");
	system_id_map.Insert(SYS_UNKNOWN, "UNKNOWN");

	organization_map.Insert(OG_XIAOJIA, "XIAOJIA");
	org2std_map[OG_XIAOJIA] = "GA-WJ";
	std_org_set.insert("GA-WJ");

	organization_map.Insert(OG_DAJIA, "DAJIA");
//	org2std_map[OG_DAJIA] = "DAJIA";
	org2std_map[OG_DAJIA] = "GA-DJ";
	std_org_set.insert("GA-DJ");

	organization_map.Insert(OG_GXB, "GXB");
	org2std_map[OG_GXB] = "GXB";
	std_org_set.insert("GXB");
	organization_map.Insert(OG_GAOYUAN, "GAOYUAN");
	org2std_map[OG_GAOYUAN] = "GY";
	std_org_set.insert("GY");
	organization_map.Insert(OG_11B, "11B");
	org2std_map[OG_11B] = "GA-11";
	std_org_set.insert("GA-11");
	organization_map.Insert(OG_12B, "12B");
	org2std_map[OG_12B] = "GA-12";
	std_org_set.insert("GA-12");
	organization_map.Insert(OG_SECURITY_M, "SECURITY_M");
	org2std_map[OG_SECURITY_M] = "SM";
	std_org_set.insert("SM");
	organization_map.Insert(OG_IS, "IS");
	org2std_map[OG_IS] = "IS";
	std_org_set.insert("IS");
	organization_map.Insert(OG_XIAOJIA_OR_11B, "XIAOJIA_OR_11B");
	org2std_map[OG_XIAOJIA_OR_11B] = "GA-WJ,GA-11";
//	std_org_set.insert();
	organization_map.Insert(OG_11B_OR_12B, "11B_OR_12B");
	org2std_map[OG_11B_OR_12B] = "GA-11,GA-12";
//	std_org_set.insert();
	organization_map.Insert(OG_GJ, "GJ");
	org2std_map[OG_GJ] = "GJ";
	std_org_set.insert("GJ");
	organization_map.Insert(OG_UNKNOWN, "UNKNOWN");
	org2std_map[OG_UNKNOWN] = "UNKNOWN";
	std_org_set.insert("UNKNOWN");

//	paramtype_map.Insert(PT_INVALID, "INVALID");
	paramtype_map.Insert(PT_QQUIN, "QQUIN");
	paramtype_map.Insert(PT_QQGROUPCODE, "QQGROUPCODE");
	paramtype_map.Insert(PT_QQGROUPUIN, "QQGROUPUIN");
	paramtype_map.Insert(PT_WEUIN, "WEUIN");
	paramtype_map.Insert(PT_WENAME, "WENAME");
	paramtype_map.Insert(PT_WEGROUPUIN, "WEGROUPUIN");
	paramtype_map.Insert(PT_MOBILE, "MOBILE");
	paramtype_map.Insert(PT_EMAIL, "EMAIL");
	paramtype_map.Insert(PT_IP, "IP");
	paramtype_map.Insert(PT_G3, "G3");
	paramtype_map.Insert(PT_WEAPPID, "WEAPPID");
	paramtype_map.Insert(PT_URL, "URL");
	paramtype_map.Insert(PT_WEGH, "WEGH");
	paramtype_map.Insert(PT_WEOPENID, "WEOPENID");
	paramtype_map.Insert(PT_IMEI, "IMEI");
	paramtype_map.Insert(PT_SEARCH_CONTENT, "SEARCHCONTENT");
	paramtype_map.Insert(PT_KEYWORDS, "KEYWORDS");
	paramtype_map.Insert(PT_ACCOUNT, "ACCOUNT");
	paramtype_map.Insert(PT_MICROBLOG_ID, "MICROBLOGID");
	paramtype_map.Insert(PT_CARDNAME, "CARDNAME");
	paramtype_map.Insert(PT_BANKCARD, "BANKCARD");
	paramtype_map.Insert(PT_DEBITCARD, "DEBITCARD");
	paramtype_map.Insert(PT_CREDITCARD, "CREDITCARD");
	paramtype_map.Insert(PT_CERTCARD, "CERTCARD");
	paramtype_map.Insert(PT_IDCARD, "IDCARD");
	paramtype_map.Insert(PT_UNKNOWN, "UNKNOWN");

	result_map.Insert(RT_OK, "OK");
	result_map.Insert(RT_FAILED, "FAILED");
	result_map.Insert(RT_TIMEOUT, "TIMEOUT");
	result_map.Insert(RT_NO_PRIVILEGE, "NO_PRIVILEGE");
	result_map.Insert(RT_HIT_WHITELIST, "HIT_WHITELIST");
	result_map.Insert(RT_FREQUENCE_CAPPING, "FREQUENCE_CAP");
}

const std::string& MapSystemID2Str(SystemID sys_id)
{
	try
	{
		return system_id_map[sys_id];
	}
	catch (...)
	{
		return UNKNOWN;
	}
}

int MapSystemID2Standard(SystemID sys_id)
{
	int the_num = static_cast<int>(sys_id);
	if (the_num >= 10000)
	{
		return the_num;
	}

	switch (sys_id)
	{
	case SYS_ZHONGJIANG_INVOICE:
	case SYS_ZHONGJIANG_ACCOUNT:
	case SYS_ZHONGJIANG:
		return STD_ZHONGJIANG;

	case SYS_531FOR_XJ:
	case SYS_531FOR_XJ_ZHONGJIANG:
	case SYS_531FOR_XIAOJIA:
	case SYS_531FOR_11B:
	case SYS_531FOR_12B:
	case SYS_531:
		return STD_531;

	case SYS_FC:
		return STD_FC;

	case SYS_PAIHAO:
		return STD_PAIHAO;

	case SYS_TONGDA:
		return STD_TONGDA;

	case SYS_XINDA:
		return STD_XINDA;

	case SYS_DIAOZHENG:
		return STD_DIAOZHENG;

	case SYS_630:
		return STD_630;

	case SYS_630_PO:
		return STD_630_PO;

	case SYS_PUSH_WEGH:
		return STD_PUSH_WEGH; // 大客户系统

	case SYS_WB:
		return STD_WB_726;

	case SYS_WB2:
		return STD_WB_LOCALE;

	case SYS_WEGH_ARTICLES:
		return STD_WEGH_ARTICLES;

	case SYS_CREDIT:
		return STD_CREDIT;

	case SYS_LBS:
		return STD_LBS;

	case SYS_SPORE:
		return STD_SPORE;

	case SYS_QQGROUP_BACKTRACE:
		return STD_QQGROUP_BACKTRACE;

	case SYS_WEGROUP_BACKTRACE:
		return STD_WEGROUP_BACKTRACE;

	case SYS_EXPRESS:
		return STD_EXPRESS;

	case SYS_GUOJIA_QQ:
		return STD_GJ_ZHONGJIANG;

	case SYS_GUOJIA_WX:
		return STD_GJ_WX_ZHONGJIANG;

	case SYS_INTERNAL_PAIHAO:
		return -1;

	case SYS_BIG:
		return 370000;

	case SYS_ANTI_TERRORISM:
	case SYS_EMERGENCYGRP_WEIDC:
	case 160:
		return 5000000;   // 信安内部系统

	case SYS_OLD_XINDA:
	case SYS_PUSH_DATA:
	case SYS_XJJS:
		std::cerr << "Deprecated system: " << sys_id << "\n";
		abort();

	case SYS_UNKNOWN:
		return 0;

	default:
		std::cerr << "Unexpected old_sys_id: " << sys_id << "\n";
		return -1;
		// abort();
	}
}

SystemID MapStr2SystemID(std::string str)
{
	return system_id_map[str];
}

const std::string& MapOrganization2Str(Organization org)
{
	return organization_map[org];
}

const std::string& MapOrganization2Standard(Organization org)
{
	auto ite = org2std_map.find(org);
	assert(ite != org2std_map.end());
	return ite->second;
}

Organization MapStr2Organization(std::string str)
{
	return organization_map[str];
}

std::map<SystemID, std::string> GetAllSysMapping()
{
	std::map<SystemID, std::string> mmm;
	const std::vector<SystemID>& all_ids = system_id_map.GetAllLeftKeys();
//	const auto& all_ids = system_id_map.GetAllLeftKeys();
	BOOST_FOREACH (const auto& id, all_ids)
	{
		mmm[id] = system_id_map[id];
	}

	return mmm;
}

const std::string& MapParamType2Str(ParamType paramtype)
{
	return paramtype_map[paramtype];
}

ParamType MapStr2ParamType(std::string str)
{
	return paramtype_map[str];
}

const std::string& MapResult2Str(Result result)
{
	return result_map[result];
}

const Result MapStr2Result(std::string str)
{
	return result_map[str];
}

bool IsStdSysIDLegal(int sys_id)
{
	return sys_id >= 10000;
}

bool IsParamTypeLegal(const std::string& param_type)
{
	return paramtype_map.IsRightKeyExisted(param_type);
}

bool IsUserOrgLegal(const std::string& user_org)
{
	std::vector<std::string> items;
	boost::split(items, user_org, boost::is_any_of(","));
	BOOST_FOREACH(const auto& item, items)
	{
		bool legal = (std_org_set.find(item) != std_org_set.end());
		if (!legal)
		{
			return false;
		}
	}

	return true;
}

bool IsResultLegal(const std::string& result)
{
	return result_map.IsRightKeyExisted(result);
}

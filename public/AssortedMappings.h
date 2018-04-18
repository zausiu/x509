/*
 * AssortedMappings.h
 *
 *  Created on: Mar 18, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef PUBLIC_ASSORTEDMAPPINGS_H_
#define PUBLIC_ASSORTEDMAPPINGS_H_

#include "gleaner.h"
#include "BiMapper.h"

enum StandardSystemID
{
	STD_ZHONGJIANG = 40000,
	STD_GJ_ZHONGJIANG = 140000,
	STD_GJ_WX_ZHONGJIANG = 150000,

	STD_531 = 30000,
	STD_FC = 60000,
	STD_PAIHAO = 70000,
	STD_TONGDA = 10000,
	STD_XINDA = 20000,
	STD_DIAOZHENG = 160000,
	STD_630 = 50000,
	STD_630_PO = 120000,
	STD_PUSH_WEGH = 130000,
	STD_WB_726 = 250000,
	STD_WB_LOCALE = 290000,
	STD_WEGH_ARTICLES = 210000,
	STD_CREDIT = 190000,
	STD_LBS = 80000,
	STD_SPORE = 170000,
	STD_QQGROUP_BACKTRACE = 300000,
	STD_WEGROUP_BACKTRACE = 350000,
	STD_EXPRESS = 260000,
};

#define SYSTEM_MACROS "'ZHONGJIANG', '531', 'FC', 'PAIHAO', 'PUSH_DATA', "    \
		       "'TONGDA', 'OLD_XINDA', 'XINDA', 'GUOJIA_QQ', 'GUOJIA_WX', "   \
	           "'ANTI_TERRORISM', 'ZHONGJIANG_INVOICE', 'ZHONGJIANG_ACCOUNT', " \
	           "'DIAOZHENG', '630_PO', 'PUSH_WEGH', 'WB', 'WB2', 'WEGH_ARTICLES', "\
	           "'CREDIT', '630', 'LBS', 'BIG', 'SPORE', 'INTERNAL_PAIHAO', "     \
			   "'QQGROUP_BACKTRACE', 'WEGROUP_BACKTRACE',"\
			   "'EXPRESS', 'EMERGENCYGRP_WEIDC', 'XJJS', '531FOR_XIAOJIA', '531FOR_11B', "     \
               "'531FOR_12B', '531FOR_XJ_ZHONGJIANG', '531FOR_XJ', 'UNKNOWN'"

const std::string& MapSystemID2Str(SystemID sys_id);
int MapSystemID2Standard(SystemID sys_id);
SystemID MapStr2SystemID(std::string str);

#define ORGNIZATION_MACROS "'XIAOJIA', 'DAJIA', 'GXB', 'GAOYUAN', '11B', "  \
                           "'12B', 'SECURITY_M', 'IS', 'XIAOJIA_OR_11B', '11B_OR_12B', "  \
						   "'UNKNOWN'"
#define ORGNIZATION_MACROS_NEO "'GA-WJ', 'DAJIA', 'GXB', 'GY', 'GA-11', " \
	                       "'GA-12', 'SM', 'IS', 'GA-WJ,GA-11', 'GA-11,GA-12',  " \
						   "'UNKNOWN'"

const std::string& MapOrganization2Str(Organization org);
const std::string& MapOrganization2Standard(Organization org);
Organization MapStr2Organization(std::string str);
std::map<SystemID, std::string> GetAllSysMapping();

#define PARAMS_TYPE_MACROS "'INVALID', 'QQUIN', 'QQGROUPCODE', 'QQGROUPUIN', 'WEUIN', " \
						"'WENAME', 'WEGROUPUIN', 'MOBILE', 'EMAIL', 'IP', 'G3', 'WEAPPID', 'URL', 'WEGH', " \
						"'WEOPENID', 'IMEI', 'CARDNAME', 'BANKCARD', 'DEBITCARD', 'CREDITCARD', " \
						"'KEYWORDS', 'ACCOUNT', " \
						"'MICROBLOGID', 'SEARCHCONTENT', 'CERTCARD', 'IDCARD', 'UNKNOWN'"

const std::string& MapParamType2Str(ParamType paramtype);
ParamType MapStr2ParamType(std::string str);

const std::string& MapResult2Str(Result result);
const Result MapStr2Result(std::string str);


bool IsStdSysIDLegal(int sys_id);
bool IsParamTypeLegal(const std::string& param_type);
bool IsUserOrgLegal(const std::string& user_org);
bool IsResultLegal(const std::string& result);

#endif /* PUBLIC_ASSORTEDMAPPINGS_H_ */

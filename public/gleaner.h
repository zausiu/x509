/*
 * gleaner.h
 *
 *  Created on: Mar 2, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef GLENER_GLEANER_H_
#define GLENER_GLEANER_H_

#include <string>

// STEP ONE: initialization
// return non-zero if failed, return 0 on success
extern "C" __attribute__((dllexport)) int GleanerInit();

enum SystemID{
	// 被多个甲方使用
	SYS_ZHONGJIANG = 2100,

	// For 小甲方
	SYS_531 = 101, // 笼统的531
	SYS_531FOR_XIAOJIA = 910, // 小甲方用53
	SYS_FC = 104, // 11局，12局也用
	SYS_PAIHAO = 111, // 排号
	SYS_PUSH_DATA = 112, // 数据推送系统
	SYS_TONGDA = 113, // 通达
	SYS_OLD_XINDA = 114, // 信达老信统
	SYS_XINDA = 115, // 信达

	// For 大甲方
	SYS_GUOJIA_QQ = 108, // 国甲QQ
	SYS_GUOJIA_WX = 109, // 国甲wechat
	SYS_ANTI_TERRORISM = 106, // 暴恐系统, gxb也用
	SYS_ZHONGJIANG_INVOICE = 116, // 中奖系统查发票
	SYS_ZHONGJIANG_ACCOUNT = 117, // 中奖系统查账号信息
	SYS_DIAOZHENG = 118,  // 调证
	SYS_WEGH_ARTICLES = 2000, // 公众号文章检索

	// For GXB
	SYS_630_PO = 103,  // 630舆情 PO: Public Opinion.
	SYS_PUSH_WEGH = 110, // 公众账号数据推送系统
	SYS_WB = 180,  // 微博
	SYS_WB2 = 181,  // 微博属地化

	// For 高院
	SYS_CREDIT = 107, // 征信

	// 11局 管网络信息
	SYS_630 = 102, // 实际上12局也用.推送微群消息
	SYS_531FOR_11B = 911, // 11局用531

	// 12局 管刑侦，有调证权
	SYS_LBS = 121, // LBS
	SYS_531FOR_12B = 912, // 12局用531

	// 自已人
	SYS_BIG = 100, // 大系统
	SYS_SPORE = 120, // 孢子
	SYS_INTERNAL_PAIHAO = 122, // 内部小组排号
	SYS_EXPRESS = 300, // 接口直通车
	SYS_EMERGENCYGRP_WEIDC = 150, // 应急组信安门户微号IDC查询
	SYS_QQGROUP_BACKTRACE = 2200, // QQ小组回溯
	SYS_WEGROUP_BACKTRACE = 2201, // 微群回溯

	// 新疆
	SYS_XJJS = 105,  // 新疆加速点
	SYS_531FOR_XJ_ZHONGJIANG = 900, // 531系统新疆中奖
	SYS_531FOR_XJ = 913,   // 新疆用531

	// 未知
	SYS_UNKNOWN = 200, // 不解释
};

enum Organization
{
	OG_XIAOJIA = 10,
	OG_DAJIA = 20,
	OG_GXB = 30,
	OG_GAOYUAN = 40,
	OG_11B = 50,
	OG_12B = 60,

	OG_XIAOJIA_OR_11B = 515,
	OG_11B_OR_12B = 556,

	OG_SECURITY_M = 1000, // 安管
	OG_IS = 1010, // 信安

	OG_GJ = 2000,  // 国甲

	OG_UNKNOWN = 9000, // 未知
};

enum ParamType
{
	PT_QQUIN = 0,
	PT_QQGROUPCODE = 6,
	PT_QQGROUPUIN = 7,

	PT_WEUIN = 1,
	PT_WENAME = 2,
	PT_WEGROUPUIN = 8,

	PT_MOBILE = 3,
	PT_EMAIL = 4,
	PT_IP = 9,
	PT_G3 = 10,
	PT_WEAPPID = 11,
	PT_URL = 12,

	PT_WEGH = 201, // 200 以前的神仙数用来兼容以前分配的数字
	PT_WEOPENID = 203, // weopen id
	PT_IMEI = 210,   // imei

	PT_SEARCH_CONTENT = 220, // 搜索什么内容
	PT_KEYWORDS = 222, // 关键字

	PT_MICROBLOG_ID = 230, // 微博ID

	PT_CARDNAME = 270, // 证件，卡的名字

	PT_BANKCARD = 280, // 银行卡号
	PT_DEBITCARD = 281, // 借记卡号，属于银行卡号的一种
	PT_CREDITCARD = 282, // 信用卡号，属于银行卡号的一种

	PT_CERTCARD = 290,  //证件号一般由国家行政机构班发
	PT_IDCARD = 291,    // 身份证号属于证件号的一种

	PT_ACCOUNT = 400,

	PT_UNKNOWN = 999  // 未知
};

enum Result
{
	RT_OK = 0,
	RT_FAILED = -1,   // 笼统的出错

	RT_NO_PRIVILEGE = -90, // 用户没有权限
	RT_HIT_WHITELIST = -99, // 命中白名单

	RT_TIMEOUT = -100,  // 超时

	RT_FREQUENCE_CAPPING = -80, // 触发频次控制
};

#define NOTSET_RETCODE 0xabcd0245
struct AccessRecord
{
	AccessRecord():retcode(NOTSET_RETCODE){}
	SystemID sys_id;
	std::string sys_ip; // 如果不填，则会用getifaddrs()拿一个IP
	std::string user_name; // 用户名
	Organization user_org;  // 用户来自的机构
	std::string user_ip;    // 用户的IP
	std::string req_cmd;    // 请求命令字，比如孢子就填10011之类，每个系统不一样。
	std::string req_content; // 具体的请求内容
	std::string req_time;  // 请求时间.北京时间 YYYY-MM-DD HH-mm-SS
	std::string resp_content; // 具体的回复内容。根据情况填关键信息
	std::string resp_time; // 回复时间
	ParamType param_type; // 请求的主要参数的类型
	std::string param;  // 实际的主要参数，比如QQ号，微信号
	Result result; // 查询结果，成功，失败，超时，受限制
	int retcode; // 每个业务自有的返回码. added at version2,default to a fairy number
	bool is_subreq; // 是否是子查询
	ParamType super_req_type; // 子查询为true时有效，此时表示父查询的类型
	std::string super_param; // 子查询为true时有效，此时表示父查询的内容
	std::string misc0;  // 万能大杂烩
	std::string misc1;  // 有两个万能大杂烩

	std::string magic;  // 魔法！除非明确知道后果，否则不要使用它！
};

// STEP TWO for AccessLog:
// return 0 if the record has been pushed into queue successfully.
// otherwise returns non-zero
extern "C" int __attribute__((dllexport)) GleanerReportAccessRecord(const AccessRecord& data);

// 这个版本的访问日志上报使用一个JSON串描述日志信息，例如：
// { “param”: 12345, 'param_type': 'QQUIN', 'retcode': 0, ...  }
// 字段名同GleanerReportAccessRecord函数的参数AccessRecord中的字段
// 但不同处有:
// 只有 sys_id, user_name, user_org, req_cmd, param_type, param, result 是必须的。
// 其它都是可选的。
// sys_id 为数字, 填写标准化后的系统名
// user_org 为字符串，填写标准化后的机构名
// param_type 为字符串，填写 'INVALID','QQUIN','QQGROUPCODE','QQGROUPUIN','WEUIN',
// 		'WENAME','WEGROUPUIN','MOBILE','EMAIL','IP','G3','WEAPPID','URL','WEGH','WEOPENID',
// 		'IMEI','CARDNAME','BANKCARD','DEBITCARD','CREDITCARD','KEYWORDS','MICROBLOGID','ACCOUNT',
// 		'SEARCHCONTENT','CERTCARD','IDCARD','UNKNOWN'
// result 为字符串，填写 'OK','FAILED','TIMEOUT','NO_PRIVILEGE','HIT_WHITELIST','FREQUENCE_CAP'
// 如果不想指定retcode则不要提供retcode字段
extern "C" int __attribute__((dllexport)) GleanerReportAccessRecordJson(const char* js);


// STEP TWO for InterfaceLog:
struct InterfaceRecord
{
	int ifid; // interface ID，直通车接口ID
	SystemID owner_sys_id; // 接口所属系统ID，一般是SYS_EXPRESS（接口直通车）
	std::string owner_sys_ip; // 接口所属系统的IP
	SystemID req_sys_id;   // 发起请求的系统的ID,比如SYS_SPORE（孢子）
	std::string req_sys_ip; // 发起请求的系统的IP
	std::string user; // 接口使用人，比如frankfan

	ParamType param_type; // 请求的主要参数的类型
	std::string param;  // 实际的主要参数，比如QQ号，微信号

	std::string req_time;  // 发起请求的时间
	std::string req_content; // 具体的请求内容
	bool deliberate_req;  // 填true表示是主观故意地执行这次接口查询，如果只是“带出来”，则填false

	std::string resp_time; // 回复时间
	std::string resp_content; // 具体的回复内容。根据情况填关键信息
	Result result; // 查询结果，成功，失败，超时，受限制等

	std::string misc;  // 万能大杂烩
};

// STEP TWO for AccessLog:
extern "C" int __attribute__((dllexport)) GleanerReportInterfaceRecord(const InterfaceRecord& data);

/////////////////////////// Advanced Usage 高级功能 ////////////////////////////////////
extern "C" bool __attribute__((dllexport)) GleanerPollIsTaskQueueEmpty();
extern "C" void __attribute__((dllexport)) GleanerJoinThrd();

#endif /* GLENER_GLEANER_H_ */

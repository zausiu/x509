/*********************************************
 * CommonHelper.h
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: www.dogeye.net
 * Created on: Jun 18, 2015
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/
#ifndef _COMMONHELPER_H_
#define _COMMONHELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <cxxabi.h>
#include <vector>
#include <boost/thread/once.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <openssl/md5.h>

//////////////////////////////////////////////////////
/// memory block
///////////////////////////////////////////////////////
typedef std::vector<char> SmartMem;
typedef boost::shared_ptr<SmartMem> SharedSmartMem;
#define DECLARE_SHARED_SMART_MEM(ptr, sz) SharedSmartMem ptr = boost::make_shared<SmartMem>(sz)

std::string DumpBinary(const unsigned char* buff, int len);

///////////////////////////////////////////////////////
/// Cgi protocol
///////////////////////////////////////////////////////

// vanguard is composed of a byte-length magic code and a 4-byte-length length field which is different from
// the concept of CgiProtocolHead
//const int VANGUARD_LEN = 5;
//
//inline int GetPacketTotalLen(void* buff)
//{
//	uint32_t len = *(uint32_t*)((char*)buff + 1);
//	return (int)len;
//}

////////////////////////////////////////////////////////////////////
/// Cryptography
///////////////////////////////////////////////////////////////////////////
void GetSHA256(const char* str, int len, unsigned char sha[64]);

struct MD5sum
{
	char md5[MD5_DIGEST_LENGTH];

	MD5sum(){}
	MD5sum(const MD5sum& another)
	{
		memcpy(md5, another.md5, sizeof(md5));
	};

	const MD5sum& operator = (const MD5sum& right)
	{
		memcpy(md5, right.md5, sizeof(md5));
		return *this;
	}

	bool operator < (const MD5sum& challenger)const
	{
		int ret = memcmp(md5, challenger.md5, sizeof(md5));
		if (ret < 0)
			return true;
		else
			return false;
	}

	bool operator == (const MD5sum& challenger)const
	{
		int ret = memcmp(md5, challenger.md5, sizeof(md5));
		return ret == 0;
	}

	std::string ToString()const;
};
inline std::size_t hash_value(const MD5sum& md5sum)
{
	std::size_t seed = *(std::size_t*)md5sum.md5;
	std::size_t residual = *(std::size_t*)(md5sum.md5 + 8);
	boost::hash_combine(seed, residual);

	return seed;
}

/////////////////////////////////////////////////////////////////////
// UniversalFunction
/////////////////////////////////////////////////////////////
typedef boost::function<void(void)> UniversalFunction;

/////////////////////////////////////////////////////////////////
// time & date
//////////////////////////////////////////////////////////////////
std::string FormatedTimestamp2FormatedDate(const char* ts);
std::string GetFormatedTimestamp(uint64_t time);
std::string GetCurrentFormatedTimestamp(); // 2015-06-29 15:22:06
std::string GetCurrentDate();  // 2015-07-28
// like the above one with additional info returned
std::string GetCurrentDateAndHMS(int& hour, int& min, int& sec, time_t& now);
std::string GetYesterdayDate();  // 2015-07-27
uint64_t FormatedTimestamp2SecondsSinceEpoch(const char* ts);
std::string SecondsSinceEpoch2FormatedTimestamp(uint64_t tv);
std::string SecondsSinceEpoch2FormatedDate(uint64_t tv);
uint64_t YYMMDD2SecondsSinceEpoch(const char* ts);
std::string YYMMDD2FormatedTimestamp(const char* ts);
std::string ReadableDuration(long duration);
bool CheckDateFormat(const char* date);
bool CheckTimestampFormat(const char* date);
bool TimestampComparer(const char* left, const char* right);

extern const boost::posix_time::ptime kMIN_PTIME;
std::string ToString(const boost::posix_time::ptime& pt);
std::string ToString(const boost::gregorian::date& date);
boost::posix_time::ptime EST8(const boost::posix_time::ptime& pt);
std::string Tomorrow(const std::string& today);

////////////////////////////////////////////////////////////////
// Network
////////////////////////////////////////////////////////////////
bool IsPrivateIP(uint32_t ip);
bool IsPrivateIP(const char* ip);
bool IsReservedIP(uint32_t ip);
bool IsReservedIP(const char* ip);
std::string IP_N2A(uint32_t ip);
std::string GetMyV4IP();

//////////////////////////////////////////////////////////////////
// Filesystem
///////////////////////////////////////////////////////////////////
void MakeSureParentDirExist(const std::string& path);
std::string MakeSureEndedWithSlash(const std::string& path);
std::string GetParentPath(const std::string& path);
std::string GetFileNameInPath(const std::string& path);
std::string TrimPathLevel(const std::string& path, int level);

/////////////////////////////////////////////////
/// charset
/////////////////////////////////////////////////
// return number greater than zero on success, otherwise return negative error code
int ConvTextUTF8ToGBK(const std::string& utf8text, std::string& gbk8text);

////////////////////////////////////////////////////
/// RTTI
/////////////////////////////////////////////////////
template<typename T> std::string GetRealFullTypeName(const T& var)
{
	int status;
	const char* mangled_name = typeid(var).name();
//	char* realname = abi::__cxa_demangle(typeid(var).name(), 0, 0, &status);
	boost::shared_ptr<char> realname(
		abi::__cxa_demangle(mangled_name, 0, 0, &status),
		free
	);

	std::string name;
	if (status == 0)
	{
		name.assign(realname.get());
		return name;
	}

	return mangled_name;
}

template<typename T> std::string GetRealTypeName(const T& var)
{
	std::string fullname = GetRealFullTypeName(var);
	size_t pos = fullname.rfind(':');
	if (pos == std::string::npos)
	{
		return fullname;
	}
	else
	{
		return fullname.substr(pos + 1);
	}
}

//////////////////////////////////////////////////////////////
// strings
////////////////////////////////////////////////////////////////
bool IsPureNumber(const std::string& str);
std::string& RemoveAllSpaces(std::string& str);

////////////////////////////////////////////////////////////////
// others
////////////////////////////////////////////////////////////////
std::string CreateUUIDStr();

#define RunOnce(code_snippet, unique_suffix) struct T##unique_suffix{ T##unique_suffix(){code_snippet;} }t##unique_suffix

#define SUPPRESS_OPTIMIZE(var) __asm__("": : ""(var))

#endif

/*********************************************
 * CommonHelper.cpp
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: blog.ykyi.net
 * Created on: Jun 18, 2015
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/
#include <openssl/sha.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <exception>
#include <string>
#include <cctype>
#include <iostream>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include "CommonHelper.h"

std::string DumpBinary(const unsigned char* buff, int len)
{
//	char buffer[4096*10];
	std::vector<char> mem(len * 2 + 1);
	char* buffer = mem.data();
	for (int i = 0; i < len; i++)
	{
		sprintf(buffer + i * 2, "%02x", buff[i]);
	}
	buffer[len * 2] = 0;

	return buffer;
}

//std::string GetBonDebugString(IS_BON::BonPtr bon)
//{
////	char buff[4096*100];
//	std::vector<char> buff(1024*1024*2);
//	bon->ToString(buff.data(), buff.size(), 1);
//
//	return buff.data();
//}

std::string MD5sum::ToString()const
{
	std::ostringstream oss;
	int sz = sizeof(md5);
	for (int i = 0; i < sz; i++)
	{
		oss << std::setfill('0') << std::setw(2) << std::hex << (int)md5[i];
	}

	return oss.str();
}

void GetSHA256(const char* str, int len, unsigned char sha[64])
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, len);
    SHA256_Final(sha, &sha256);
}

std::string FormatedTimestamp2FormatedDate(const char* ts)
{
	const char* ptr = strchr(ts, ' ');
	if (ptr == NULL)
	{
		boost::format fmt("Timestamp has to in the form: 'YYYY-MM-DD HH::mm::SS, but received:[%s]");
		std::string info = (fmt % ts).str();
		throw std::runtime_error(info);
	}

	return std::string(ts, ptr);
}

std::string GetFormatedTimestamp(uint64_t time)
{
	struct tm result;
	localtime_r((const time_t*)&time, &result);

	char buff[256];
	strftime(buff, sizeof(buff), "%F %T", &result);

	return buff;
}

std::string GetCurrentFormatedTimestamp()
{
	struct tm result;
	time_t now = time(NULL);

	return GetFormatedTimestamp(now);
}

std::string GetCurrentDate()
{
	struct tm result;
	time_t now = time(NULL);

	localtime_r(&now, &result);

	char buff[256];
	strftime(buff, sizeof(buff), "%F", &result);

	return buff;
}

std::string GetCurrentDateAndHMS(int& hour, int& min, int& sec, time_t& now)
{
	struct tm result;
	now = time(NULL);

	localtime_r(&now, &result);

	char buff[256];
	strftime(buff, sizeof(buff), "%F", &result);

	hour = result.tm_hour;
	min = result.tm_min;
	sec = result.tm_sec;

	return buff;
}

std::string GetYesterdayDate()
{
	struct tm result;
	time_t now = time(NULL) - 24 * 60 * 60;

	localtime_r(&now, &result);

	char buff[256];
	strftime(buff, sizeof(buff), "%F", &result);

	return buff;
}

uint64_t FormatedTimestamp2SecondsSinceEpoch(const char* ts)
{
	struct tm tm;

	memset(&tm, 0, sizeof(tm));
	strptime(ts, "%Y-%m-%d %H:%M:%S", &tm);

//	char* s = asctime(&tm);
	time_t t = mktime(&tm);  // localtime
//	time_t t = timegm(&tm);

	return t;
}

std::string SecondsSinceEpoch2FormatedDate(uint64_t tv)
{
	const std::string& ts = SecondsSinceEpoch2FormatedTimestamp(tv);
	return FormatedTimestamp2FormatedDate(ts.c_str());
}

std::string SecondsSinceEpoch2FormatedTimestamp(uint64_t tv)
{
	char buff[32];
	struct tm tm;

	if (tv == 0)
	{
		return "";
	}

//	gmtime_r((time_t*)&tv, &tm);
	localtime_r((time_t*)&tv, &tm);

	if (strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm))
	{
		return buff;
	}

	return "ERR";
}

uint64_t YYMMDD2SecondsSinceEpoch(const char* ts)
{
	struct tm tm;

	memset(&tm, 0, sizeof(tm));
	strptime(ts, "%Y%m%d", &tm);

	time_t t = timegm(&tm);

	return t;
}

std::string YYMMDD2FormatedTimestamp(const char* ts)
{
	struct tm tm;

	memset(&tm, 0, sizeof(tm));
	strptime(ts, "%Y%m%d", &tm);

	char buff[32];
	if (strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm))
	{
		return buff;
	}

	return "ERR";
}

std::string ReadableDuration(long duration)
{
	int hours = duration / (60 * 60);
	int minutes = (duration - hours * 60 * 60) / 60;
	int seconds = duration % 60;

	std::string str = boost::str(boost::format("%d时%d分%d秒") % hours % minutes % seconds);

	return str;
}

bool CheckDateFormat(const char* date)
{
	static boost::regex format("\\d{4}-\\d{2}-\\d{2}");
	bool matched = boost::regex_match(date, format);
	return matched;
}

bool CheckTimestampFormat(const char* ts)
{
	static boost::regex format("\\d{4}-\\d{1,2}-\\d{1,2} \\d{1,2}:\\d{1,2}:\\d{1,2}");
	bool matched = boost::regex_match(ts, format);
	return matched;
}

bool TimestampComparer(const char* left, const char* right)
{
	return FormatedTimestamp2SecondsSinceEpoch(left) < FormatedTimestamp2SecondsSinceEpoch(right);
}

const boost::posix_time::ptime kMIN_PTIME(boost::date_time::min_date_time);

std::string ToString(const boost::posix_time::ptime& pt)
{
	if (pt == kMIN_PTIME)
	{
		return "";
	}

	// %Y-%m-%d %H:%M:%S the format if use facet
	const boost::gregorian::date& date = pt.date();
	std::ostringstream oss;
	oss << date.year() << '-' << static_cast<int>(date.month()) << '-' << date.day()
		<< ' ' << pt.time_of_day();
	return oss.str();
}

std::string ToString(const boost::gregorian::date& date)
{
	std::ostringstream oss;
	oss << date.year() << '-' << static_cast<int>(date.month()) << '-' << date.day();

	return oss.str();
}

boost::posix_time::ptime EST8(const boost::posix_time::ptime& pt)
{
	static const boost::posix_time::time_duration d(8, 0, 0);
	return pt + d;
}

std::string Tomorrow(const std::string& today)
{
	boost::gregorian::date d = boost::gregorian::from_simple_string(today);
	boost::gregorian::day_iterator ite(d);
	++ite;
	const std::string& tomorrow = (boost::format("%04i-%02i-%02i")
		% ite->year() % static_cast<int>(ite->month()) % ite->day()).str();
	return tomorrow;
}

bool IsPrivateIP(uint32_t ip)
{
	uint8_t b1, b2, b3, b4;
	b1 = (uint8_t)(ip >> 24);
	b2 = (uint8_t)((ip >> 16) & 0x0ff);
	b3 = (uint8_t)((ip >> 8) & 0x0ff);
	b3 = (uint8_t)(ip & 0x0ff);

	// 10.x.y.z
	if (b1 == 10)
		return true;

	// 172.16.0.0 - 172.31.255.255
	if ((b1 == 172) && (b2 >= 16) && (b2 <= 31))
		return true;

	// 192.168.0.0 - 192.168.255.255
	if ((b1 == 192) && (b2 == 168))
		return true;

	return false;
}

bool IsReservedIP(uint32_t ip)
{
	uint8_t b1, b2, b3, b4;
	b1 = (uint8_t)(ip >> 24);
	b2 = (uint8_t)((ip >> 16) & 0x0ff);
	b3 = (uint8_t)((ip >> 8) & 0x0ff);
	b3 = (uint8_t)(ip & 0x0ff);

	if (b1 == 0)
		return true;

	if (b1 == 127)
		return true;

	if ((b1 == 169) && (b2 == 254))
		return true;

	if ((b1 == 255) && (b2 == 255) && (b3 == 255) && (b4 == 255))
		return true;

	return false;
}

bool IsPrivateIP(const char* ip)
{
	in_addr_t tmp = inet_addr(ip);
	uint32_t ip_num = *(uint32_t*)&tmp;
	bool b = IsPrivateIP(htonl(ip_num));

	return b;
}

bool IsReservedIP(const char* ip)
{
	in_addr_t tmp = inet_addr(ip);
	uint32_t ip_num = *(uint32_t*)&tmp;
	bool b = IsReservedIP(htonl(ip_num));

	return b;
}

std::string IP_N2A(uint32_t ip)
{
	struct in_addr ip_addr;
	ip_addr.s_addr = ip;

	return inet_ntoa(ip_addr);
}

std::string GetMyV4IP()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];
	std::string the_ip;

	if (getifaddrs(&ifaddr) == -1)
	{
		//perror("getifaddrs");
		goto _exit;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

		if((strncmp(ifa->ifa_name, "eth", 3)==0 || strncmp(ifa->ifa_name, "ens", 3)==0)
					&&(ifa->ifa_addr->sa_family==AF_INET))
		{
			if (s != 0)
			{
				//fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
				goto _exit;
			}
			//printf("\tInterface : <%s>\n",ifa->ifa_name);
			//printf("\t  Address : <%s>\n", host);
			the_ip = host;
		}
	}

_exit:
	freeifaddrs(ifaddr);

	return the_ip;
}

void MakeSureParentDirExist(const std::string& path)
{
	std::string path_copy(path);
	bool absolute = (path_copy[0] == '/');

	boost::regex dir_expr("([-_0-9a-zA-z]+)/");
	boost::regex_token_iterator<std::string::iterator> ite(path_copy.begin(), path_copy.end(), dir_expr, 0);
	boost::regex_token_iterator<std::string::iterator> end;

	std::string parent;
	while (ite != end)
	{
		std::string sub_dir = *ite++;
		//std::cout << sub_dir << std::endl;
		parent += sub_dir;

		// /foo/bar/baz/qux would be split in 3 parts, foo/ bar/ baz/,
		// and the leading / should be prepended for purpose of creating directory successfully
		if (absolute && parent[0] != '/')
		{
			parent = '/' + parent;
		}
		//std::cout << parent << "\n";
		mkdir(parent.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
		// if the above line of code failed
		chmod(parent.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
	}
	umask(S_IWGRP|S_IWOTH);
}

std::string MakeSureEndedWithSlash(const std::string& path)
{
	//if (!path.empty() && path.back() != '/')
	if (!path.empty() && *path.rbegin() != '/')
	{
		return path + "/";
	}

	return path;
}

std::string GetParentPath(const std::string& path)
{
	boost::filesystem::path p(path);

	return p.parent_path().string();
}

std::string GetFileNameInPath(const std::string& path)
{
	boost::filesystem::path p(path);

	return p.filename().string();
}

std::string TrimPathLevel(const std::string& path, int level)
{
	char buff[1024*2];
	int num_solidus_hit = 0;

	if (level <= 0)
	{
		return path;
	}

	int n = snprintf(buff, sizeof(buff), "%s", path.c_str());

	for (int i = 0; i < n; i++)
	{
		if (buff[i] == '/')
		{
			num_solidus_hit++;

			if (num_solidus_hit == level)
			{
				const char* pos = buff + i + 1;
				return pos;
			}
		}
	}

	return "";
}

int ConvTextUTF8ToGBK(const std::string& utf8text, std::string& gbktext)
{
	iconv_t cd = iconv_open("gbk"/*to code*/, "utf-8"/*from code*/);
	assert((iconv_t)-1 != cd);

	int utf8text_len = utf8text.length();
	std::string result;
	result.resize(utf8text_len + 1);

	char* inbuf = const_cast<char*>(utf8text.c_str());
	size_t inbytesleft = utf8text_len;
	char* outbuf = const_cast<char*>(result.c_str());
	size_t outbytesleft = result.length();
	memset(outbuf, 0, outbytesleft);

	size_t retcode = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
	if (retcode == E2BIG)
	{
		std::cerr << "There is not sufficient room at *outbuf.";
	}
	else if (retcode == EILSEQ)
	{
		std::cerr << "An invalid multibyte sequence has been encountered in the input.";
	}
	else if (retcode == EINVAL)
	{
		std::cerr << " An incomplete multibyte sequence has been encountered in the input.";
	}

	iconv_close(cd);

	if (retcode > 0)
	{
		gbktext = result;
	}

	return retcode;
}

bool IsPureNumber(const std::string& str)
{
	int count = strspn(str.c_str(), "0123456789");
	if (count > 0 && count == str.size())
	{
		return true;
	}

	return false;
}

std::string& RemoveAllSpaces(std::string& str)
{
	std::string::iterator ite = std::remove_if(str.begin(), str.end(), ::isspace);
	str.erase(ite, str.end());

	return str;
}

std::string CreateUUIDStr()
{
	boost::uuids::random_generator generator;
	boost::uuids::uuid uuid = generator();
	std::string uuid_str = to_string(uuid);

	return uuid_str;
}

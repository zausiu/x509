/*
 * gleaner.cpp
 *
 *  Created on: Mar 2, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wblog.ykyi.net 
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include "json/json.h"
//#include "Protocols.h"
#include "gleaner.h"
#include "qos_client.h"

#define ENDL static_cast<std::ostream&(*)(std::ostream&)>(std::endl)

using namespace cl5;

void locate_logsvr(std::string& svr_ip, unsigned short& port);

struct GleanerHead
{
	unsigned int magic_code;
	int json_len;
	uint64_t reserved;
};
#define MAGIC_CODE 0x12345678

////////////////////////////////////// LOGGER ////////////////////////////////////
class GleanerLogger
{
private:
	std::ofstream ofs_;
	int lines_count_;
	bool active_;
	static const int MAX_LINE_NUM = 100000;

public:
	GleanerLogger()
	{
		active_ = (access("/tmp/gleaner_log_on", F_OK) != -1);
		if (!active_)
			return;

		open();
	}
	template<typename T> GleanerLogger& operator << (const T& v)
	{
		if (!active_)
			return *this;

		ofs_ << v;
		if (++lines_count_ > MAX_LINE_NUM)
		{
			ofs_.close();
			open();
		}

		return *this;
	}
	template<typename T> GleanerLogger& operator << (const T* v)
	{
		if (!active_)
			return *this;

		ofs_ << v;
		if (++lines_count_ > MAX_LINE_NUM)
		{
			ofs_.close();
			open();
		}

		return *this;
	}
	void open()
	{
		if (!active_)
			return;

		lines_count_ = 0;
		ofs_.open("/tmp/gleaner.log");
	}
	~GleanerLogger()
	{}
}g_gleaner_logger;

////////////////////////////////////// L5 //////////////////////////////////////////
struct L5Route
{
	L5Route()
	{
		qos_request._modid = 64200385;
		qos_request._cmd = 65536;
		getroute_retcode = -1;
		status_code = 0;
		sock_fd = 0;
		gettimeofday(&start, NULL);
	}

	~L5Route()
	{
		if (getroute_retcode < 0)
		{
			g_gleaner_logger << "Failed to call ApiGetRoute(), so no need to call update" << ENDL;
			return;
		}

		gettimeofday(&end, NULL);
		int timecost = (int)((end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);

		std::string errmsg;
		int ret = ApiRouteResultUpdate(qos_request, status_code, timecost, errmsg);
		if (ret != 0)
		{
			g_gleaner_logger << "Failed to call ApiRouteResultUpdate, ret:" << ret << ENDL;
		}

		g_gleaner_logger << "reported status_code: " << status_code << " to l5 agent." << ENDL;
	}

	int GetSockFd()
	{
		assert(sock_fd != -1);
		return sock_fd;
	}

	void InformBadFd();

	QOSREQUEST qos_request;

	int getroute_retcode;
	struct timeval start, end;
	int status_code;
	int sock_fd;
};

class L5Balancer
{
public:
	L5Route* GetL5Route()
	{
		std::string err_msg;
		L5Route* route = new L5Route;
		int& ret = route->getroute_retcode;
		ret = ApiGetRoute(route->qos_request, 0.2, err_msg);
		if (ret != 0 && saved_ip_.empty())
		{
			g_gleaner_logger << err_msg << ENDL;
			std::cerr << "L5 doesn't work. cos: " << err_msg << "\n";
			assert(0);
		}

		if (ret == 0)
		{
			saved_ip_ = route->qos_request._host_ip;
			saved_port_ = route->qos_request._host_port;
			g_gleaner_logger << "ApiGetRoute() succeeded: "
					<< saved_ip_ << ":" << saved_port_ << ENDL;
		}

		route->sock_fd = GetSockFd(saved_ip_, saved_port_);

		return route;
	}

	void InformBadFd(int fd)
	{
		close(fd);
		g_gleaner_logger << "close fd:" << fd << ENDL;
		for (std::map<uint64_t, int>::iterator ite = fd_map_.begin();
			 ite != fd_map_.end();
			 ++ite)
		{
			if (ite->second == fd)
			{
				fd_map_.erase(ite);
				g_gleaner_logger << "Found and erased fd: " << fd << ENDL;
				break;
			}
		}
	}

	int GetSockFd(std::string& ip, unsigned short port)
	{
		unsigned int ip_n;
		inet_pton(AF_INET, saved_ip_.c_str(), &ip_n);

		uint64_t key = ip_n;
		key <<= 32;
		key |= saved_port_;

		std::map<uint64_t, int>::iterator ite = fd_map_.find(key);
		if (ite != fd_map_.end())
		{
			int sockfd = ite->second;
			g_gleaner_logger << "Picked already created fd: " << sockfd << ENDL;
			return sockfd;
		}

		struct sockaddr_in server_addr;
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			g_gleaner_logger << "failed to call socket(), cos:" << strerror(errno) << ENDL;
			abort();
		}
		g_gleaner_logger << "created a new fd: " << sockfd << ENDL;
		fd_map_[key] = sockfd;

		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = ip_n;
		server_addr.sin_port = htons(port);

		int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
		if (ret != 0)
		{
			g_gleaner_logger << "Failed to connect to: " << ip << ":" << port << ENDL;
		}

		return sockfd;
	}

private:
	std::string saved_ip_;
	unsigned short saved_port_;

	std::map<uint64_t, int> fd_map_;
}g_l5balancer;

void L5Route::InformBadFd()
{
	status_code = -1;
	g_l5balancer.InformBadFd(sock_fd);
}


////////////////////////////////////// TheGleaner //////////////////////////////////////

class RecordBase
{
public:
	virtual std::string ConvertRecord2JsonStr() = 0;
	virtual ~RecordBase(){};
};

class TheGleaner
{
public:
	TheGleaner();
	virtual ~TheGleaner();

	int Init();
	void Uninit();
	int Report(RecordBase* record);
	int Report_sync(RecordBase* record);
	bool PollIsTaskQueueEmpty();

protected:
	static void* ThreadEntry(void* arg);
	void* ActualThreadEntry();
//	int Connect_yellowpagesvr(); // using yellow_page_server
	int SendRecord(RecordBase* record);

private:
	pthread_t thread_h_;
	volatile bool quit_;
	int thread_ok_rc_;

	std::list<RecordBase*> records_;
	std::list<RecordBase*> ready_records_;
	static const int MAX_RECORDS_CAPACITY = 2000000;
	int records_size_;
	pthread_mutex_t records_mutex_h_;
	pthread_cond_t evt_cond_h_;
};

#define CHECK_QUIT(q, ret) do{ if(q) return ret; }while(0)

TheGleaner::TheGleaner()
{
	quit_ = false;
	thread_ok_rc_ = -1;
	records_size_ = 0;
}

TheGleaner::~TheGleaner()
{
	Uninit();
}

int TheGleaner::Init()
{
	quit_ = false;

	srand(time(NULL));

	int ret = pthread_mutex_init(&records_mutex_h_, NULL);
	if (ret != 0)
	{
		return ret;
	}
	ret = pthread_cond_init(&evt_cond_h_, NULL);
	if (ret != 0)
	{
		return ret;
	}

	thread_ok_rc_ = pthread_create(&thread_h_, NULL, ThreadEntry, this);
	if (thread_ok_rc_ != 0)
	{
		return thread_ok_rc_;
	}

	return 0;
}

void TheGleaner::Uninit()
{
	if (thread_ok_rc_ != 0)
	{
		return;
	}

	if (!quit_)
	{
		quit_ = true;
		pthread_cond_signal(&evt_cond_h_);
		pthread_join(thread_h_, NULL);
		pthread_mutex_destroy(&records_mutex_h_);
		pthread_cond_destroy(&evt_cond_h_);
		thread_ok_rc_ = -1;
	}
}


int TheGleaner::Report(RecordBase* record)
{
	pthread_mutex_lock(&records_mutex_h_);
	if (records_size_ > MAX_RECORDS_CAPACITY)
	{
		pthread_mutex_unlock(&records_mutex_h_);
		return -2;
	}
	records_.push_back(record);
	records_size_++;
	pthread_mutex_unlock(&records_mutex_h_);

	return 0;
}

int TheGleaner::Report_sync(RecordBase* record)
{
	ActualThreadEntry();
	return 0;
}

bool TheGleaner::PollIsTaskQueueEmpty()
{
	bool empty;
	pthread_mutex_lock(&records_mutex_h_);
	empty = (ready_records_.empty() && records_.empty());
	pthread_mutex_unlock(&records_mutex_h_);

	return empty;
}

void* TheGleaner::ThreadEntry(void* arg)
{
	TheGleaner* ptr = (TheGleaner*)arg;
	return ptr->ActualThreadEntry();
}

void* TheGleaner::ActualThreadEntry()
{
	//printf("Launched a new thread.\n");
	struct timespec tspec;
//	Connect_yellowpagesvr();

	while (!quit_ || !PollIsTaskQueueEmpty())
	{
		pthread_mutex_lock(&records_mutex_h_);
		while (records_.empty() && !quit_)
		{
			clock_gettime(CLOCK_REALTIME, &tspec);
			tspec.tv_sec += 1;
			pthread_cond_timedwait(&evt_cond_h_, &records_mutex_h_, &tspec);
		}

		if (!records_.empty())
		{
			ready_records_.swap(records_);
			records_size_ = 0;
		}
        pthread_mutex_unlock(&records_mutex_h_);

    	for (std::list<RecordBase*>::iterator ite = ready_records_.begin();
    		 ite != ready_records_.end();
    		 )
    	{
    		int ret = SendRecord(*ite);
    		if (ret == 0)
    		{
    			pthread_mutex_lock(&records_mutex_h_);
    			ready_records_.erase(ite++);
    			pthread_mutex_unlock(&records_mutex_h_);
    			//printf("send ok\n");
    		}
    		else
    		{
    			CHECK_QUIT(quit_ && PollIsTaskQueueEmpty(), NULL);
//    			Connect_yellowpagesvr();
    		}
    	}

    	CHECK_QUIT(quit_ && PollIsTaskQueueEmpty(), NULL);
	}
}

// use L5 instead
//int TheGleaner::Connect_yellowpagesvr()
//{
//	int ret;
//	std::string server_ip;
//	unsigned short server_port;
//
//	do
//	{
//		locate_logsvr(server_ip, server_port);
//
//		struct sockaddr_in server_addr;
//		sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
//		if (sockfd_ < 0)
//		{
//			perror("ERROR opening socket");
//			abort();
//		}
//		memset(&server_addr, 0, sizeof(server_addr));
//		server_addr.sin_family = AF_INET;
//		server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
//		server_addr.sin_port = htons(server_port);
//
//		ret = connect(sockfd_, (struct sockaddr*)&server_addr, sizeof(server_addr));
//		if (ret < 0)
//		{
//			g_gleaner_logger << "connect failed: " << strerror(errno) << ENDL;
//			CHECK_QUIT(quit_, -1);
//			sleep(1);
//			//perror("ERROR connect");
//		}
//		CHECK_QUIT(quit_, -1);
//	}while(ret != 0);
//
//	return 0;
//}

int TheGleaner::SendRecord(RecordBase* record)
{
	L5Route* route = g_l5balancer.GetL5Route();
	int sockfd = route->GetSockFd();

	const std::string& json_str = record->ConvertRecord2JsonStr();
	GleanerHead vanguard;
	vanguard.magic_code = MAGIC_CODE;
	vanguard.json_len = json_str.length();
	const char* pos_ary[2] = {(const char*)&vanguard, json_str.data()};
	int len_ary[2] = {sizeof(vanguard), vanguard.json_len};

	for (int i = 0; i < 2; i++)
	{
		const char* pos = pos_ary[i];
		int len = len_ary[i];
		while (len > 0)
		{
			int ret = send(sockfd, pos, len, MSG_NOSIGNAL);
			if (ret < 0)
			{
				route->InformBadFd();
				delete route;
				return ret;
			}
			pos += ret;
			len -= ret;
		}
	}

	delete record;
	delete route;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////

static TheGleaner* g_gleaner;
std::string my_host_ip;

class Cleaner
{
public:
	Cleaner(){}
	~Cleaner()
	{
		if (g_gleaner != NULL)
		{
			g_gleaner->Uninit();
			delete g_gleaner;
		}
	}
}g_cleaner;

std::string PickIP()
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

		if((strncmp(ifa->ifa_name, "eth", 3) == 0 || strncmp(ifa->ifa_name, "ens", 3) == 0 ) &&
				(ifa->ifa_addr->sa_family == AF_INET))
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

// Fist this function randomly choose a yellow_page_server.
// Then it ask the yellow_page_server what the log server ip and port are.
// return 0 on success or non-zero on failure
int try_to_locate_logsvr(std::string& svr_ip, unsigned short& port)
{
	const char* yellow_page_svrs[] = {"g.ica.cm.com", "ica.cm.com"};
	int sz = sizeof(yellow_page_svrs) /sizeof(char*);

	int rc, fd, ret;
	struct addrinfo hints;
	struct addrinfo* result, *rp;
	char buff[512];
	const char* yellow_svr_domain_name;

	ret = -1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	do
	{
		int rand_num = rand();
		g_gleaner_logger << "Generated a rand number:" << rand_num << ENDL;
		int idx = rand_num % sz;
		yellow_svr_domain_name = yellow_page_svrs[idx];
		g_gleaner_logger << "Begin to do name/address translation for " << yellow_svr_domain_name << ENDL;
		rc = getaddrinfo(yellow_svr_domain_name, "6580", &hints, &result);
		if (rc != 0)
		{
			g_gleaner_logger << "getaddrinfo: " << gai_strerror(rc) << "[errcode:" << rc << "]" << ENDL;
//			freeaddrinfo(result);
		}
	}
	while(rc != 0);

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		struct sockaddr_in* addr_in = (struct sockaddr_in*)rp->ai_addr;
		unsigned short the_port = ntohs(addr_in->sin_port);
		const char* v4ip = inet_ntop(rp->ai_family, &addr_in->sin_addr, buff, sizeof(buff));
		if (v4ip == NULL)
		{
			g_gleaner_logger << "encountered wrong format v4ip" << ENDL;
			continue;
		}

		g_gleaner_logger << "Try to connect to yellow_page_server " << v4ip << ":" << the_port  << ENDL;

		fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (fd == -1)
		{
			g_gleaner_logger << "failed to create socket: " << strerror(errno) << ENDL;
			continue;
		}

		rc = connect(fd, rp->ai_addr, rp->ai_addrlen);
		if (rc == -1)
		{
			g_gleaner_logger << "failed to call connect: " << strerror(errno) << ENDL;
			goto close_and_loop;
		}

		rc = send(fd, "JK-TEST", 7, 0);
		if (rc == -1)
		{
			g_gleaner_logger << "failed to call send: " << strerror(errno) << ENDL;
			goto close_and_loop;
		}

		rc = recv(fd, buff, sizeof(buff), 0);
		if (rc == -1)
		{
			g_gleaner_logger << "failed to call recv: " << strerror(errno) << ENDL;
			goto close_and_loop;
		}
		g_gleaner_logger << "received [" << buff << "] from yellow_page_server "
				  << yellow_svr_domain_name << "\n";
		if (rc < 9)
		{
			g_gleaner_logger << "bad response format from yellow_page_server." << ENDL;
			goto close_and_loop;
		}
		buff[rc] = 0;

		{
			std::istringstream iss(buff);
			iss >> svr_ip >> port;
		}

		ret = 0;

		continue;

close_and_loop:
		close(fd);
	}

	freeaddrinfo(result);

	return ret;
}

void locate_logsvr(std::string& svr_ip, unsigned short& port)
{
	int ret;
	do
	{
		g_gleaner_logger << "call locate_logsvr" << ENDL;
		ret = try_to_locate_logsvr(svr_ip, port);
	}
	while (ret != 0);
	g_gleaner_logger << "successfully located logsvr: " << svr_ip << ":" << port << ENDL;
}


int GleanerInit()
{
	my_host_ip = PickIP();
	g_gleaner = new TheGleaner;
	return g_gleaner->Init();
}

class ARecord: public RecordBase
{
public:
	ARecord(){data_.retcode = 0xabcd0245;}
	virtual std::string ConvertRecord2JsonStr();

	AccessRecord data_;
	const static int VERSION = 3;
};

class AJRecord: public RecordBase
{
public:
	AJRecord(const char* js)
	{
		Json::Reader reader;
		bool ok = reader.parse(js, jv_root_, false);
		if (!ok)
			return;

		jv_root_["__VERSION"] = "RAWJSON";
		jv_root_["__TYPE"] = "ACCESSLOG";

		const std::string& sys_ip = jv_root_["sys_ip"].asString();
		if (sys_ip.empty())
		{
			jv_root_["sys_ip"] = my_host_ip;
		}

		const char* time_fields[] = {"req_time", "resp_time"};
		for (int i = 0; i < 2; i++)
		{
			const char* field = time_fields[i];
			if (jv_root_[field].isNull())
			{
				struct tm result;
				time_t now = time(NULL);
				localtime_r((const time_t*)&now, &result);

				char buff[256];
				strftime(buff, sizeof(buff), "%F %T", &result);
				jv_root_[field] = buff;
			}
		}
	}

	bool IsOK()const
	{
		return !jv_root_.isNull();
	}

	virtual std::string ConvertRecord2JsonStr()
	{
		Json::FastWriter writer;
		const std::string js_txt = writer.write(jv_root_);
		std::cout << js_txt << "\n";
		return js_txt;
	}

private:
	Json::Value jv_root_;
};

std::string ARecord::ConvertRecord2JsonStr()
{
	Json::Value root;
	root["sys_id"] = data_.sys_id;
	root["sys_ip"] = (data_.sys_ip.empty() ?  my_host_ip : data_.sys_ip);
	root["user_name"] = data_.user_name;
	root["user_org"] = data_.user_org;
	root["user_ip"] = data_.user_ip;
	root["req_cmd"] = data_.req_cmd;
	root["req_content"] = data_.req_content;
	root["req_time"] = data_.req_time;
	root["resp_content"] = data_.resp_content;
	root["resp_time"] = data_.resp_time;
	root["param_type"] = data_.param_type;
	root["param"] = data_.param;
	root["result"] = data_.result;
	root["retcode"] = data_.retcode;
	root["is_subreq"] = data_.is_subreq;
	root["super_req_type"] = data_.super_req_type;
	root["super_param"] = data_.super_param;
	root["misc0"] = data_.misc0;
	root["misc1"] = data_.misc1;
	root["magic"] = data_.magic;

	root["__VERSION"] = VERSION;
	root["__TYPE"] = "ACCESSLOG";

	Json::FastWriter fast_writer;
	std::string compact_str = fast_writer.write(root);

	return compact_str;
}

int GleanerReportAccessRecord(const AccessRecord& data)
{
	ARecord* arecord = new ARecord();
	arecord->data_ = data;
	int ret = g_gleaner->Report(arecord);

	return ret;
}

int GleanerReportAccessRecord_sync(const AccessRecord& data)
{
	ARecord* arecord = new ARecord();
	arecord->data_ = data;
	int ret = g_gleaner->Report_sync(arecord);

	return ret;
}

int GleanerReportAccessRecordJson(const char* js)
{
	AJRecord* ajrecord = new AJRecord(js);
	if (!ajrecord->IsOK())
	{
		std::cerr << "Detected json syntax error.\n";
		return -1;
	}

	int ret = g_gleaner->Report(ajrecord);

	return ret;
}

class IfRecord: public RecordBase
{
public:
	virtual std::string ConvertRecord2JsonStr();

	InterfaceRecord data_;
	const static int VERSION = 1;
};

std::string IfRecord::ConvertRecord2JsonStr()
{
	Json::Value root;
	root["ifid"] = data_.ifid;
	root["owner_sys_id"] = data_.owner_sys_id;
	root["owner_sys_ip"] = data_.owner_sys_ip;
	root["req_sys_id"] = data_.req_sys_id;
	root["req_sys_ip"] = data_.req_sys_ip;
	root["user"] = data_.user;
	root["param_type"] = data_.param_type;
	root["param"] = data_.param;
	root["req_time"] = data_.req_time;
	root["req_content"] = data_.req_content;
	root["deliberate_req"] = data_.deliberate_req;
	root["resp_time"] = data_.resp_time;
	root["resp_content"] = data_.resp_content;
	root["result"] = data_.result;
	root["misc"] = data_.misc;

	root["__VERSION"] = VERSION;
	root["__TYPE"] = "INTERFACELOG";

	Json::FastWriter fast_writer;
	std::string compact_str = fast_writer.write(root);

	return compact_str;
}

int GleanerReportInterfaceRecord(const InterfaceRecord& data)
{
	IfRecord* ifrecord = new IfRecord;
	ifrecord->data_ = data;
	int ret = g_gleaner->Report(ifrecord);

	return ret;
}

bool GleanerPollIsTaskQueueEmpty()
{
	bool empty = g_gleaner->PollIsTaskQueueEmpty();

	return empty;
}


void GleanerJoinThrd()
{
	g_gleaner->Uninit();
}

//////////////////////////////////////////////////////////////
/// This l5 wrapper maintains a tcp connection pool.
/// Written by kamuszhou@tencent.com
/// Website http://blog.ykyi.net/
/// Created on March 13, 2018
//////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <map>
#include "cpp_wrapper_l5.h"

#define ENDL static_cast<std::ostream&(*)(std::ostream&)>(std::endl)

using namespace cl5;

namespace MyL5
{

class DebugLogger
{
private:
    std::ofstream ofs_;
    int lines_count_;
    bool active_; 
    static const int MAX_LINE_NUM = 100000; 

public: 
    DebugLogger() 
    { 
        active_ = (access("/tmp/cpp_wrapper_l5_log_on", F_OK) != -1); 
        if (!active_) 
            return;
        
        open(); 
    } 
    
    template<typename T>  DebugLogger& operator << (const T& v) 
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
   
    template<typename T> DebugLogger& operator << (const T* v) 
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
        ofs_.open("/tmp/cpp_wrapper_l5.log"); 
    } 
    
    ~DebugLogger() 
    {}
}g_debug_logger;


L5Route::L5Route(int modid, int cmd, L5Balancer& l5blc):l5b(l5blc)
{ 
    qos_request._modid = modid; 
    qos_request._cmd = cmd; 
    getroute_retcode = -1; 
    status_code = 0; 
    sock_fd = 0;

    gettimeofday(&start, NULL); 
}

L5Route::~L5Route()
{ 
    if (getroute_retcode < 0) 
    { 
        g_debug_logger << "Failed to call ApiGetRoute(), so no need to call update" << ENDL; 
        return; 
    } 
        
    gettimeofday(&end, NULL); 
    int timecost = (int)((end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000); 
    std::string errmsg; 
    int ret = ApiRouteResultUpdate(qos_request, status_code, timecost, errmsg); 
        
    if (ret != 0) 
    { 
        g_debug_logger << "Failed to call ApiRouteResultUpdate, ret:" << ret << ENDL; 
    }

    g_debug_logger << "A L5Route is released.\nReported status_code: " << status_code << " to l5 agent." << ENDL;
}

int L5Route::GetSockFd()
{ 
    g_debug_logger << "sock_fd is " << sock_fd << "\n"; 
    assert(sock_fd != -1); 
    return sock_fd; 
}

L5Balancer::L5Balancer(int modid, int cmd)
{
    modid_ = modid;
    cmd_ = cmd;
}

L5Route* L5Balancer::GetL5Route()
{
    std::string err_msg;
    L5Route* route = new L5Route(modid_, cmd_, *this);
    g_debug_logger << "created a new route: " << route << " " << std::dec << (unsigned long)route << "\n";
        
    int& ret = route->getroute_retcode;
    ret = ApiGetRoute(route->qos_request, 0.2, err_msg); 
    if (ret != 0 && saved_ip_.empty()) 
    { 
        g_debug_logger << err_msg << ENDL; 
        std::cerr << "L5 doesn't work. cos: " << err_msg << "\n"; 
        assert(0); 
    }
            
    if (ret == 0) 
    { 
        saved_ip_ = route->qos_request._host_ip; 
        saved_port_ = route->qos_request._host_port; 
    	route->ip = saved_ip_;
    	route->port = saved_port_;
            
        g_debug_logger << "ApiGetRoute() succeeded: " 
            << saved_ip_ << ":" << saved_port_ << ENDL; 
    }

    route->sock_fd = GetSockFd(saved_ip_, saved_port_); 

    return route; 
}

void L5Balancer::ReleaseL5Route(L5Route* route)
{
    delete route;
}

int L5Balancer::Recv(L5Route* route, void* buff, int len, int flags)
{
   int fd = route->sock_fd;
   int ret = recv(fd, buff, len, flags);
   if (ret < 0)
   {
       char buff[1024] = {0};
       strerror_r(errno, buff, sizeof(buff));
       g_debug_logger << "Failed to recv data from socket fd: " << route->sock_fd << buff << ENDL;
       route->InformBadFd();
   }

   return ret;
}

void L5Balancer::InformBadFd(int fd) 
{ 
    close(fd); 
    g_debug_logger << "close fd:" << fd << ENDL; 
    for (std::map<uint64_t, int>::iterator ite = fd_map_.begin(); 
         ite != fd_map_.end(); 
         ++ite) 
    { 
        if (ite->second == fd) 
        { 
            fd_map_.erase(ite); 
            g_debug_logger << "Found and erased fd: " << fd << ENDL; 
            break; 
        } 
    }
}

int L5Balancer::GetSockFd(std::string& ip, unsigned short port) 
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
        g_debug_logger << "Picked already created fd: " << sockfd << ENDL; 
        return sockfd; 
    } 

    struct sockaddr_in server_addr; 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    
    if (sockfd < 0) 
    { 
        g_debug_logger << "failed to call socket(), cos:" << strerror(errno) << ENDL; 
        abort(); 
    } 
    g_debug_logger << "created a new fd: " << sockfd << ENDL; 
    fd_map_[key] = sockfd; 
    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = ip_n; 
    server_addr.sin_port = htons(port); 
    
    int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)); 
    if (ret != 0) 
    { 
        g_debug_logger << "Failed to connect to: " << ip << ":" << port << ENDL; 
    } 
    
    return sockfd; 
}

int L5Balancer::Send(L5Route* route, char* data, int len)
{
    int remained = len;
    for (int i = 0; i < remained;)
    {
        int ret = send(route->sock_fd, data + i, len - i, MSG_NOSIGNAL);
        if (ret < 0)
        {
            char buff[1024] = {0};
            strerror_r(errno, buff, sizeof(buff));
            g_debug_logger << "Failed to send data to socket fd: " << route->sock_fd << buff << ENDL;

            route->InformBadFd();
            return -1;
        }

        i += ret;
    }

    return 0;
}

void L5Route::InformBadFd()
{
    status_code = -1;
    l5b.InformBadFd(sock_fd);
}


#define CHECK_QUIT(q, ret) do{ if(q) return ret; }while(0)

Gleaner::Gleaner(L5Balancer* l5b)
{
	quit_ = false;
	thread_ok_rc_ = -1;
	records_size_ = 0;
	l5b_ = l5b;
}

Gleaner::~Gleaner()
{
	Uninit();
}

int Gleaner::Init()
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

void Gleaner::Uninit()
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


int Gleaner::Send(std::vector<char>& data)
{
	pthread_mutex_lock(&records_mutex_h_);
	if (records_size_ > MAX_RECORDS_CAPACITY)
	{
		pthread_mutex_unlock(&records_mutex_h_);
		return -2;
	}
	data_lst_.emplace_back(std::move(data));

	records_size_++;
	pthread_mutex_unlock(&records_mutex_h_);

	return 0;
}

bool Gleaner::PollIsTaskQueueEmpty()
{
	bool empty;
	pthread_mutex_lock(&records_mutex_h_);
	empty = (data_lst_.empty() && ready_data_lst_.empty());
	pthread_mutex_unlock(&records_mutex_h_);

	return empty;
}

void* Gleaner::ThreadEntry(void* arg)
{
	Gleaner* ptr = (Gleaner*)arg;
	return ptr->ActualThreadEntry();
}

void* Gleaner::ActualThreadEntry()
{
	//printf("Launched a new thread.\n");
	struct timespec tspec;
//	Connect_yellowpagesvr();

	while (!quit_ || !PollIsTaskQueueEmpty())
	{
		pthread_mutex_lock(&records_mutex_h_);
		while (data_lst_.empty() && !quit_)
		{
			clock_gettime(CLOCK_REALTIME, &tspec);
			tspec.tv_sec += 1;
			pthread_cond_timedwait(&evt_cond_h_, &records_mutex_h_, &tspec);
		}

		if (!data_lst_.empty())
		{
			ready_data_lst_.swap(data_lst_);
			records_size_ = 0;
		}
        pthread_mutex_unlock(&records_mutex_h_);

    	for (std::list<std::vector<char>>::iterator ite = ready_data_lst_.begin();
    		 ite != ready_data_lst_.end();
    		 )
    	{
    		int ret = SendCore(*ite);
    		if (ret == 0)
    		{
    			pthread_mutex_lock(&records_mutex_h_);
    			ready_data_lst_.erase(ite++);
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

int Gleaner::SendCore(std::vector<char>& data)
{
	L5Route* route = l5b_->GetL5Route();
	int sockfd = route->GetSockFd();

	int len = data.size();
	const char* pos = data.data();
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

	delete route;
	return 0;
}

};

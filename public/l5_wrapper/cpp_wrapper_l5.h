//////////////////////////////////////////////////////////////
/// This l5 wrapper maintains a tcp connection pool.
/// Written by kamuszhou@tencent.com
/// Website http://blog.ykyi.net/
/// Created on March 13, 2018
//////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <map>
#include <list>
#include <pthread.h>
#include <qos_client.h>

/** WARNING: NOT THREAD SAFE */

namespace MyL5
{

class L5Balancer;

struct L5Route
{
    L5Route(int modid, int cmd, L5Balancer& l5blc); 
    ~L5Route();

    int GetSockFd();
    void InformBadFd();

    cl5::QOSREQUEST qos_request;
    int getroute_retcode;
    struct timeval start, end; 
    int status_code; 
    int sock_fd;

    std::string ip;
    unsigned short port;

    L5Balancer& l5b;
};

class L5Balancer
{
friend class L5Route;
public:
    L5Balancer(int modid, int cmd);

    L5Route* GetL5Route();
    void ReleaseL5Route(L5Route* route);

    /**
     * Take best efforts to send all data.
     * This call returns 0 if all data has been sent. 
     * On error, -1 is returned.
     */
    int Send(L5Route* route, char* data, int len);
    /**
     * A simple wrapper of recv system call
     */
    int Recv(L5Route* route, void* buff, int len, int flags);


private:
    void InformBadFd(int fd);
    int GetSockFd(std::string& ip, unsigned short port);

private:
    std::string saved_ip_;
    unsigned short saved_port_;
    std::map<uint64_t, int> fd_map_;

    int modid_;
    int cmd_;
};

class Gleaner
{
public:
	Gleaner(L5Balancer* l5b);
	virtual ~Gleaner();

	int Init();
	void Uninit();
	// data will be swapped out
	int Send(std::vector<char>& data);
	bool PollIsTaskQueueEmpty();

protected:
	static void* ThreadEntry(void* arg);
	void* ActualThreadEntry();
	int SendCore(std::vector<char>& data);

private:
	pthread_t thread_h_;
	volatile bool quit_;
	int thread_ok_rc_;

	std::list<std::vector<char>> data_lst_;
	std::list<std::vector<char>> ready_data_lst_;
	static const int MAX_RECORDS_CAPACITY = 2000000;
	int records_size_;
	pthread_mutex_t records_mutex_h_;
	pthread_cond_t evt_cond_h_;

	L5Balancer* l5b_;
};

};

/*
 * OtterImpl.h
 *
 *  Created on: Apr 19, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef OTTER_OTTERIMPL_H_
#define OTTER_OTTERIMPL_H_

#include <map>
#include <string>
#include <pthread.h>
#include "qos_client.h"
#include "x509/X509Crt.h"
#include "x509/RSAPrivateKey.h"
#include "KeyNSalt.h"

///////////////////////////////////////////////////////
/// Deliberately slow server !!!!!!!!!!!
/////////////////////////////////////////////////////////
class OtterImpl
{
public:
	OtterImpl();
	virtual ~OtterImpl();

	void init(const char* crt_path, const char* key_path);

	int get_key_and_salt(int year, int month, int day, int hour,
			char* key, char* salt);

private:
	void update_l5_balance(std::string* ip, unsigned short* port);
	void report_l5(int retcode);
	int establish_short_tcp_conn(const std::string& ip, unsigned short port);
	std::string build_request(int year, int month, int day, int hour);
	int send_request(int fd, const std::string& req);
	int recv_response(int fd, std::string* resp);

	void get_key_and_salt(const std::string& uid, char* key, char* salt);

private:
	pthread_mutex_t mutex_;

	X509Crt x509_crt_;
    RSAPrivateKey* key_;

	static const int modid_ = 64200385;
	static const int cmd_ = 196608;

	cl5::QOSREQUEST qos_request_;
	struct timeval start_tv_;
	struct timeval end_tv_;

	std::map<std::string, KeyNSalt> kns_map_;
};

#endif /* OTTER_OTTERIMPL_H_ */

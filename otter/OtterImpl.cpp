/*
 * OtterImpl.cpp
 *
 *  Created on: Apr 19, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <stdexcept>
#include <iostream>
#include "spdlog/fmt/fmt.h"
#include "json/json.h"
#include "Base64.h"
#include "NeatUtilities.h"
#include "OtterImpl.h"

OtterImpl::OtterImpl()
{
}

OtterImpl::~OtterImpl()
{
	pthread_mutex_destroy(&mutex_);
    delete key_;
}

void OtterImpl::init(const char* crt_path, const char* key_path)
{
	x509_crt_.init(crt_path);
    key_ = new RSAPrivateKey(key_path);

	int ret = pthread_mutex_init(&mutex_, nullptr);
	if (ret != 0)
	{
		throw std::runtime_error("Failed to create mutex.");
	}

	qos_request_._modid = modid_;
	qos_request_._cmd = cmd_;
}

int OtterImpl::get_key_and_salt(int year, int month, int day, int hour,
			char* key, char* salt)
{
	if (year < 2018 || year > 2030)
	{
		std::cerr << "Bad year argument.\n";
		return -1;
	}
	if (month < 1 || month > 12)
	{
		std::cerr << "Bad month argument.\n";
		return -1;
	}
	if (day < 0 || day > 31)
	{
		std::cerr << "Bad day argument.\n";
		return -1;
	}
	if (hour < 0 || hour > 24)
	{
		std::cerr << "Bad hour argument.\n";
		return -1;
	}


	pthread_mutex_lock(&mutex_);

	const std::string& uid = form_dayhour_uid(year, month, day, hour);
	auto ite = kns_map_.find(uid);
	if (ite != kns_map_.end())
	{
		get_key_and_salt(uid, key, salt);
		return 0;
	}

	std::string ip;
	unsigned short port;

	int sockfd;
	while (true)
	{
		while (true)
		{
			update_l5_balance(&ip, &port);
			sockfd = establish_short_tcp_conn(ip, port);
			if (sockfd < 0)
			{
				report_l5(sockfd);
				usleep(1000);
			}
			else
			{
				break;
			}
		}

		const std::string& req = build_request(year, month, day, hour);

		int ret = send_request(sockfd, req);
		if (ret < 0)
		{
			report_l5(ret);
			close(sockfd);
			continue;
		}

		std::string resp;
		ret = recv_response(sockfd, &resp);
		if (ret < 0)
		{
			report_l5(ret);
			close(sockfd);
			continue;
		}
		else
		{
			report_l5(0);
			close(sockfd);

			Json::Value jv_root;
			Json::Reader reader;
			if (reader.parse(resp, jv_root, false))
			{
				KeyNSalt kns;

				const std::string& key_b64 = jv_root["key"].asString();
				const std::string& salt_b64 = jv_root["salt"].asString();
 
                const std::string& tmpk = key_->unceal_text_base64(key_b64.c_str());
                memcpy(kns.key, tmpk.data(), sizeof(kns.key));

                const std::string& tmps = key_->unceal_text_base64(salt_b64.c_str());
                memcpy(kns.salt, tmps.data(), sizeof(kns.salt));

                if (kns_map_.size() > 24)
                {
                        kns_map_.clear();
                }

				kns_map_[uid] = kns;

				break;  // successfully got response
			}
			else
			{
				continue;
			}
		}
	}

	get_key_and_salt(uid, key, salt);
	pthread_mutex_unlock(&mutex_);

	return 0;
}

void OtterImpl::update_l5_balance(std::string* ip, unsigned short* port)
{
	using namespace cl5;

	std::string err_msg;
	int ret = ApiGetRoute(qos_request_, 0.2, err_msg);
	if (ret != 0)
	{
		throw std::runtime_error(fmt::format("L5 doesn't work. cos: {}", err_msg));
	}

	*ip = qos_request_._host_ip;
	*port = qos_request_._host_port;

	gettimeofday(&start_tv_, nullptr);
}

void OtterImpl::report_l5(int retcode)
{
	gettimeofday(&end_tv_, NULL);
	int timecost = (int)((end_tv_.tv_sec - start_tv_.tv_sec) * 1000 + (end_tv_.tv_usec - start_tv_.tv_usec) / 1000);

	std::string err_msg;
	int ret = ApiRouteResultUpdate(qos_request_, retcode, timecost, err_msg);
	if (ret != 0)
	{
		std::cerr << "Failed to call ApiRouteResultUpdate, ret: " << ret << std::endl;
	}
}

int OtterImpl::establish_short_tcp_conn(const std::string& ip, unsigned short port)
{
	struct sockaddr_in server_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		throw std::runtime_error(fmt::format("Failed to create socket fd, cos: {}", strerror(errno)));
	}

	unsigned int ip_n;
	inet_pton(AF_INET, ip.c_str(), &ip_n);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = ip_n;
	server_addr.sin_port = htons(port);

	int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (ret != 0)
	{
		std::string err_msg = fmt::format("Failed to connect to: {}:{}", ip, port);
		std::cerr << err_msg << std::endl;
		close(sockfd);

		return -1;
	}

	return sockfd;
}

std::string OtterImpl::build_request(int year, int month, int day, int hour)
{
	Json::Value jv_root;
	jv_root["crt"] = x509_crt_.as_str();
	jv_root["year"] = year;
	jv_root["month"] = month;
	jv_root["day"] = day;
	jv_root["hour"] = hour;

	std::string str = jv_root.asString();

	return str;
}

int OtterImpl::send_request(int sockfd, const std::string& req)
{
	int len = req.size();
	int ret = send_all(sockfd, (const char*)&len, sizeof(len));

	return 0;
}

int OtterImpl::recv_response(int fd, std::string* resp)
{
	int len;
	int ret = recv(fd, &len, sizeof(len), MSG_WAITALL);
	if (ret < sizeof(len))
	{
		return -1;
	}

	std::string& resp_str = *resp;
	resp_str.resize(len);
	ret = recv(fd, const_cast<char*>(resp_str.data()), len, MSG_WAITALL);
	if (ret < len)
	{
		return -1;
	}

	return 0;
}

void OtterImpl::get_key_and_salt(const std::string& uid, char* key, char* salt)
{
	const KeyNSalt& kns = kns_map_.at(uid);

	memcpy(key, kns.key, sizeof(kns.key));
	memcpy(salt, kns.salt, sizeof(kns.salt));
}

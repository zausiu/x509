/*
 * KeyCache.h
 *
 *  Created on: 2017年1月3日
 *      Author: kamuszhou
 *       Email: kamuszhou@tencent.com
 *     website: http://blog.ykyi.net
 */

#ifndef MEKKA_KEYCACHE_H_
#define MEKKA_KEYCACHE_H_

#include <string>
#include <map>
#include <vector>
#include <czmq.h>

struct KS
{
	KS()
	{}
	KS(const char* k, const char* s)
	{
		memcpy(key, k, 32);
		memcpy(salt, s, 32);
		memset(pad0, 0, 32);
		memset(pad1, 1, 32);
		memset(pad2, 2, 32);
	}
	char pad0[32];
	char key[32];
	char pad1[32];
	char salt[32];
	char pad2[32];

	std::string KeyToString()const;
	std::string SaltToString()const;
};

class KeyCache
{
	friend void MyActorEntry(zsock_t* pipe, void* args);
public:
	KeyCache();
	virtual ~KeyCache();

	void Init(std::string dolphin_public_key, std::string my_cert_path, std::string dolphin_eps,
			int hour_ks_limit, int day_ks_limit);

	void GetKS(const std::string& date, const std::string& hour, KS& ks);

	static void MyActorEntry(zsock_t* pipe, void* args);

private:
	void MyActorActualEntry(zsock_t* pipe);
	void FetchKS(const std::string& date, const std::string& hour, KS& ks);
	bool FetchKSFromCache(const std::string& date, const std::string& hour, KS& ks);

	std::string CombineMapKey(const std::string& date, const std::string& hour)
	{
		return date + hour;
	}

private:
	zactor_t* actor_;
	zsock_t* dealer_;
	zsock_t* pipe_;
	zcert_t* my_cert_;
	zpoller_t* poller_;

	std::map<std::string, KS> hour_ks_map_;
	std::map<std::string, KS> day_ks_map_;

	std::string dolphin_public_key_;
	std::string my_cert_path_;
	std::string dolphin_eps_;

	int hour_ks_limit_;
	int day_ks_limit_;
};

#endif /* MEKKA_KEYCACHE_H_ */

/*
 * KeyCache.cpp
 *
 *  Created on: 2017年1月3日
 *      Author: kamuszhou
 *       Email: kamuszhou@tencent.com
 *     website: http://blog.ykyi.net
 */
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/scope_exit.hpp>
#include "json/json.h"
#include "KeyCache.h"
#include "CommonHelper.h"
#include "Base64.h"
#include "SnazzyLogger.h"

std::string KS::KeyToString()const
{
	return DumpBinary((const unsigned char*)key, sizeof(key));
}

std::string KS::SaltToString()const
{
	return DumpBinary((const unsigned char*)salt, sizeof(salt));
}

KeyCache::KeyCache()
{
	actor_ = NULL;
	dealer_ = NULL;
	my_cert_ = NULL;
	poller_ = NULL;
	hour_ks_limit_ = 0;
	day_ks_limit_ = 0;
}

KeyCache::~KeyCache()
{
	zactor_destroy(&actor_);
	zsock_destroy(&dealer_);
	zcert_destroy(&my_cert_);
	zpoller_destroy(&poller_);
}

void KeyCache::MyActorEntry(zsock_t* pipe, void* args)
{
	KeyCache* ptr = static_cast<KeyCache*>(args);
	ptr->MyActorActualEntry(pipe);
}

void KeyCache::Init(std::string dolphin_public_key, std::string my_cert_path, std::string dolphin_eps,
		int hour_ks_limit, int day_ks_limit)
{
	dolphin_public_key_ = dolphin_public_key;
	my_cert_path_ = my_cert_path;
	dolphin_eps_ = dolphin_eps;

	hour_ks_limit_ = hour_ks_limit;
	day_ks_limit_ = day_ks_limit;

	__sync_synchronize();
	actor_ = zactor_new(KeyCache::MyActorEntry, this);
	assert(actor_ != NULL);
}

void KeyCache::GetKS(const std::string& date, const std::string& hour, KS& ks)
{
	assert(hour.size() == 2);
	int ret = zstr_sendx(actor_, "KS", date.c_str(), hour.c_str(), NULL);
	assert(ret == 0);

	zmsg_t* msg = zmsg_recv(actor_);
	char* d = zmsg_popstr(msg);
	char* h = zmsg_popstr(msg);
	zframe_t* k = zmsg_pop(msg);
	zframe_t* s = zmsg_pop(msg);

	assert(d == date);
	assert(h == hour);
	assert(zframe_size(k) == sizeof(ks.key));
	assert(zframe_size(s) == sizeof(ks.salt));
	memcpy(ks.key, zframe_data(k), sizeof(ks.key));
	memcpy(ks.salt, zframe_data(s), sizeof(ks.salt));

	zstr_free(&d);
	zstr_free(&h);
	zframe_destroy(&k);
	zframe_destroy(&s);
	zmsg_destroy(&msg);
}

void KeyCache::MyActorActualEntry(zsock_t* pipe)
{
	pipe_ = pipe;

	dealer_ = zsock_new(ZMQ_DEALER);

	zuuid_t* uuid = zuuid_new();
	std::string suuid = zuuid_str(uuid);
	zuuid_destroy(&uuid);
	std::string myid = GetMyV4IP() + "_" + suuid;
	zsock_set_identity(dealer_, myid.c_str());
	zsock_set_curve_serverkey(dealer_, dolphin_public_key_.c_str());

	my_cert_ = zcert_load(my_cert_path_.c_str());
	if (my_cert_ == 0)
	{
		std::cerr << "Failed to load certificate from " << my_cert_path_ << "\n";
		abort();
	}
	zcert_apply(my_cert_, dealer_);

	std::vector<std::string> eps;
	boost::split(eps, dolphin_eps_, boost::is_any_of(", "), boost::token_compress_on);
	int ret = 0;
	BOOST_FOREACH(const std::string& ep, eps)
	{
		if (ep.empty())
			continue;

		ret = zsock_connect(dealer_, "%s", ep.c_str());
	}

	poller_ = zpoller_new(dealer_, NULL);
	assert(poller_ != NULL);
//	zpoller_set_nonstop(poller_, true);
	// at present, initialization for this actor has been finished
	zsock_signal(pipe, 1);

	while (true)
	{
		char* cmd, *date, *hour;
		BOOST_SCOPE_EXIT(&cmd, &date, &hour)
		{
			zstr_free(&cmd);
			zstr_free(&date);
			zstr_free(&hour);
		}BOOST_SCOPE_EXIT_END

		int ret = zstr_recvx(pipe, &cmd, &date, &hour, NULL);
		if (ret == -1)
		{
			std::cerr << "Not expected to reach here.\n";
			abort();
		}
		if (ret == 0)
		{
			continue;
		}
		if (streq(cmd, "$TERM"))
		{
			return;
		}
		assert(streq(cmd, "KS"));
		assert(ret == 3);
		assert(strlen(hour) == 2);

		KS ks;
		FetchKS(date, hour, ks);

		zmsg_t* msg = zmsg_new();
		assert(msg != NULL);
		zmsg_addstr(msg, date);
		zmsg_addstr(msg, hour);
		zmsg_addmem(msg, ks.key, sizeof(ks.key));
		zmsg_addmem(msg, ks.salt, sizeof(ks.salt));
		ret = zmsg_send(&msg, pipe);
		assert(ret == 0);
	}
}

void KeyCache::FetchKS(const std::string& date, const std::string& hour, KS& ks)
{
	bool ok = FetchKSFromCache(date, hour, ks);
	if (ok)
	{
		return;
	}

	// send request and parse response until successful
	while (true)
	{
		Json::Value jv_root;
		Json::Value& jv_filter = jv_root["filter"];
		jv_filter["date"] = date;
		jv_filter["start_req_time"] = hour;
		jv_filter["end_req_time"] = hour;

		const std::string& ask4ks = jv_root.toStyledString();
		zstr_send(dealer_, ask4ks.c_str());

		LOG->info("send request to dolphin: {}", ask4ks.c_str());

		// read from dealer until no message is there
		while (true)
		{
			void* sk = zpoller_wait(poller_, 250);
//			bool expired = zpoller_expired(poller_);
//			bool terminated = zpoller_terminated(poller_);
//			std::cout << expired << " " << terminated << std::endl;
			if (sk != NULL)
			{
				char* js_resp = zstr_recv(dealer_);
				if (js_resp == NULL)
				{
					std::cerr << "zstr_recv(dealer) returns unexpected NULL\n";
					abort();
					continue;
				}
				Json::Reader rdr;
				Json::Value jv_resp;
				bool ok = rdr.parse(js_resp, jv_resp, false);
				if (ok)
				{
					LOG->info("Received ks from dolphin: {}", js_resp);

					const Json::Value& lks = jv_resp["lks"];
					const auto& member_names = lks.getMemberNames();
					BOOST_FOREACH (const std::string& map_key, member_names)
					{
						KS tmp;

						const std::string& key_b64 = lks[map_key]["key"].asString();
						const std::string& salt_b64 = lks[map_key]["salt"].asString();
						int len = Base64decode(tmp.key, key_b64.c_str());
						assert(len == 32);

						len = Base64decode(tmp.salt, salt_b64.c_str());
						assert(len == 32);

						std::string h(map_key.data() + 10);
						if (h != "24")
						{
							if (hour_ks_limit_ > 0 && hour_ks_map_.size() > hour_ks_limit_)
							{
								hour_ks_map_.clear();
								LOG->info("cleared all hour ks.");
							}
							hour_ks_map_[map_key] = tmp;
						}
						else
						{
							if (day_ks_limit_ > 0 && day_ks_map_.size() > day_ks_limit_)
							{
								day_ks_map_.clear();
								LOG->info("cleared all day ks.");
							}
							day_ks_map_[map_key] = tmp;
						}

						std::string temp_str;
						LOG->info("populate ks belongs to %s:", map_key.c_str());
						temp_str = tmp.KeyToString();
						LOG->info("key: %s", temp_str.c_str());
						temp_str = tmp.SaltToString();
						LOG->info("salt: %s", temp_str.c_str());
					}
				}
				zstr_free(&js_resp);
			}
			else
			{
				break;
			}
		}

		bool ok = FetchKSFromCache(date, hour, ks);
		if (ok)
		{
			return;
		}
	}
}

bool KeyCache::FetchKSFromCache(const std::string& date, const std::string& hour, KS& ks)
{
	std::string map_key = CombineMapKey(date, hour);
	if (hour != "24")
	{
		auto ite = hour_ks_map_.find(map_key);
		if (ite != hour_ks_map_.end())
		{
			ks = ite->second;
			return true;
		}
	}
	else
	{
		auto ite = day_ks_map_.find(map_key);
		if (ite != day_ks_map_.end())
		{
			ks = ite->second;
			return true;
		}
	}

	return false;
}

/*
 * ZooVisitor.cpp
 *
 *  Created on: 2016年12月23日
 *      Author: kamuszhou
 *       Email: kamuszhou@tencent.com
 *     website: http://blog.ykyi.net
 */
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/random/random_device.hpp>
#include <KamusCrypto.h>
#include "ZooVisitor.h"
#include "Base64.h"
#include "CommonHelper.h"
//#include "../mylogger/MyLogger.h"
#include "SnazzyLogger.h"

const int MAX_ZNODE_DATA_LEN = 4096;
const char* ZK_USER_PASSWD = "kamuszhou@user_jk";
// echo -n kamuszhou:kamuszhou@user_jk | openssl dgst -binary -sha1 | openssl base64
const char* ZK_USER_PASSWD_DIGEST = "3hWWRHop65XBFJ98aoHdWVTXcU8=";
const unsigned char* ZK_DATA_PASSWD = (const unsigned char*)"\x12\x13\x21\xf1\xf1\x43\x76\xFb\xbf\xe0\x0e\x8f\x78\x98\x88\x65" \
					   	   	   	      "\x49\x79\xa0\xac\xc3\xdd\xcc\xce\xec\x5e\x5f\xcc\x03\xdd\xdf\x3b";

ZooVisitor::ZooVisitor()
{
	zhandle_ = 0;
}

ZooVisitor::~ZooVisitor()
{
}

void InitEventWatcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
	if (state == ZOO_CONNECTED_STATE)
	{
		std::cout << "Connection to zookeeper has been established.\n";
	}
	else
	{
		std::cout << "received state: " << state << "\n";
	}
}

void AddAuthCompletion(int rc, const void *data)
{
	assert(rc == ZOK);
	LOG->info("AddAuth OK");
}

void ZooVisitor::Init()
{
	zhandle_ = zookeeper_init("localhost:2181", InitEventWatcher, 10000/*recv timeout*/, NULL, NULL, 0);
	if (zhandle_ == NULL)
	{
		std::cerr << "zookeeper_init failed: " << strerror(errno) << "\n";
		abort();
	}

	AddAuth();
	MakeSurePathExist("/cuttle");
}

Json::Value ZooVisitor::GetZnodeData(const std::string& date, int hour)
{
	const std::string& path = (boost::format("/cuttle/%s/%d") % date % hour).str();

	return GetZnodeData(path);
}

Json::Value ZooVisitor::GetZnodeData(const std::string& path)
{
	std::string znode_data;

	char buff[MAX_ZNODE_DATA_LEN];
	int buff_len = sizeof(buff);
	struct Stat stat;
	int ret = zoo_get(zhandle_, path.c_str(), 0/*watch*/, buff, &buff_len, &stat);
	if (ret != ZOK)
	{
		MakeSurePathExist(path);
		int ret = zoo_get(zhandle_, path.c_str(), 0/*watch*/, buff, &buff_len, &stat);
		if (ret != ZOK)
		{
			LOG->error("Failed to get znode data with path: {} cos: {} {}", path, ret, zerror(ret));
			abort();
		}
	}

	int version = stat.version;
	if (buff_len <= 0)
	{
		// Try to set the data
		unsigned char key[32];
		unsigned char salt[32];

		GetCryptoSecureRandomBytes(key, sizeof(key));
		GetCryptoSecureRandomBytes(salt, sizeof(salt));

		const std::string& key_base64 = g_crypto.EncryptBase64(key, sizeof(key), ZK_DATA_PASSWD);
		const std::string& salt_base64 = g_crypto.EncryptBase64(salt, sizeof(salt), ZK_DATA_PASSWD);

		Json::Value jv;
		jv["key"] = key_base64;
		jv["salt"] = salt_base64;
		const std::string& new_znode_data = jv.toStyledString();

		int ret = zoo_set(zhandle_, path.c_str(), new_znode_data.c_str(), new_znode_data.size(), version);
		if (ret == ZOK)
		{
			znode_data = new_znode_data;
		}
		else if (ret == ZBADVERSION)
		{
			char data[MAX_ZNODE_DATA_LEN];
			int data_len = sizeof(data);
			int ret = zoo_get(zhandle_, path.c_str(), 0/*watch*/, data, &data_len, &stat);
			if (ret != ZOK)
			{
//				LOG << fmt("Failed to get znode data with path after detected data exist: %s cos: %d %s\n") % path % ret % zerror(ret);
				LOG->error("Failed to get znode data with path after detected data exist: {} cos: {} {}", path, ret, zerror(ret));
				abort();
			}
			znode_data.assign(data, data + data_len);
		}
		else
		{
//			LOG << fmt("Failed to set znode data with path: %s cos: %d %s\n") % path % ret % zerror(ret);
			LOG->error("Failed to set znode data with path: %s cos: {} {}\n", path, ret, zerror(ret));
			abort();
		}
	}
	else
	{
		znode_data.assign(buff, buff + buff_len);
	}

	Json::Value jv_data;
	Json::Reader reader;
	bool ok = reader.parse(znode_data, jv_data, false);
	assert(ok);

	const std::string& base64_encrypted_key = jv_data["key"].asString();
	const std::string& base64_encrypted_salt = jv_data["salt"].asString();
	std::vector<char> true_key(base64_encrypted_key.size());
	std::vector<char> true_salt(base64_encrypted_salt.size());
	int true_key_len = g_crypto.DecryptBase64(base64_encrypted_key.c_str(), (unsigned char*)true_key.data(), ZK_DATA_PASSWD);
	int true_salt_len = g_crypto.DecryptBase64(base64_encrypted_salt.c_str(), (unsigned char*)true_salt.data(), ZK_DATA_PASSWD);
	assert(true_key_len == 32);
	assert(true_salt_len == 32);

	int base64_true_key_len = Base64encode_len(32);
	int base64_true_salt_len = Base64encode_len(32);
	std::vector<char> base64_true_key(base64_true_key_len);
	std::vector<char> base64_true_salt(base64_true_salt_len);
	Base64encode(base64_true_key.data(), true_key.data(), true_key_len);
	Base64encode(base64_true_salt.data(), true_salt.data(), true_salt_len);
	jv_data["key"] = base64_true_key.data();
	jv_data["salt"] = base64_true_salt.data();

	return jv_data;
}

void ZooVisitor::AddAuth()
{
	std::string cert = (boost::format("kamuszhou:%s") % ZK_USER_PASSWD).str();
	int ret = zoo_add_auth(zhandle_, "digest", cert.c_str(), cert.size(), NULL, NULL);
	assert(ret == ZOK);
}

void ZooVisitor::MakeSurePathExist(const std::string& path)
{
	boost::filesystem::path the_path(path);
	if (the_path.has_parent_path())
	{
		const std::string& parent_path = the_path.parent_path().c_str();
		if (parent_path != "/")
		{
			MakeSurePathExist(parent_path);
		}
	}

	LOG->info("Begin to create path: {}", path);

	std::string id = (boost::format("kamuszhou:%s") % ZK_USER_PASSWD_DIGEST).str();
	struct ACL acl[] = {
		{
			ZOO_PERM_CREATE|ZOO_PERM_READ|ZOO_PERM_WRITE|ZOO_PERM_ADMIN,
				{(char*)"digest", (char*)id.c_str()}
		}
	};
	struct ACL_vector acl_vec = {1, acl};
	char path_buff[256];
	int ret = zoo_create(zhandle_, path.c_str(), NULL, -1, &acl_vec, 0, path_buff, sizeof(path_buff));
	if (ret == ZNODEEXISTS || ret == ZOK)
	{
		return;
	}
	else
	{
//		LOG << fmt("Failed to create znode with path: %s cos: %d %s\n") % path % ret % zerror(ret);
		LOG->error("Failed to create znode with path: {} cos: {} {}", path, ret, zerror(ret));
		abort();
	}
}

void ZooVisitor::GetCryptoSecureRandomBytes(unsigned char* buff, int len)
{
	boost::random_device rd;
	int times = len / 4;
	assert(len % 4 == 0);
	for (int i = 0; i < times; i++)
	{
		*(unsigned int*)(buff + i * 4) = rd();
	}
}

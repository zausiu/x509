/*
 * ZooVisitor.h
 *
 *  Created on: 2016年12月23日
 *      Author: kamuszhou
 *       Email: kamuszhou@tencent.com
 *     website: http://blog.ykyi.net
 */

#ifndef DOLPHIN_ZOOVISITOR_H_
#define DOLPHIN_ZOOVISITOR_H_

#include <json/json.h>
#include <zookeeper/zookeeper.h>

class ZooVisitor
{
public:
	ZooVisitor();
	virtual ~ZooVisitor();

	void Init();

	Json::Value GetZnodeData(const std::string& date, int hour);

private:
	Json::Value GetZnodeData(const std::string& path);

	void AddAuth();
	void MakeSurePathExist(const std::string& path);
	void GetCryptoSecureRandomBytes(unsigned char* buff, int len);

private:
	zhandle_t* zhandle_;
};

#endif /* DOLPHIN_ZOOVISITOR_H_ */

/*
 * Courier.cpp
 *
 *  Created on: Feb 23, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#include <stdlib.h>
#include <sstream>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include "Courier.h"
#include "seclog/log.h"

Courier g_courier;

Courier::Courier()
{
}

Courier::~Courier()
{
}

void Courier::Init(const std::string& path)
{
	courier_path_ = path;

	// testing code
//  char cmd[256] = { 0 };
//	for (int i = 0; i < 5; i++)
//	{
//		snprintf(cmd, sizeof(cmd), "cmd%d", i);
//		std::cout << cmd << std::endl;
//		AsyncSendMessage(cmd);
//	}
}

void Courier::AsyncAlarm(const std::string& msg_text)
{
	boost::async(boost::launch::async,
			boost::bind(&Courier::Alarm, this, msg_text)
	);
}

void Courier::Alarm(std::string msg_text)
{
	std::ostringstream oss;

	oss << courier_path_ << " alarm \"" << msg_text << "\"";

	const std::string& cmd = oss.str();

	std::cout << "Launch command: " << cmd << std::endl;
//	LOG_DBG("Invoke courier with command: %s", cmd.c_str());
	::system(cmd.c_str());
}

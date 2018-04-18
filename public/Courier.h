/*
 * Courier.h
 *
 *  Created on: Feb 23, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef SPORE_COURIER_H_
#define SPORE_COURIER_H_

class Courier;
extern Courier g_courier;

class Courier
{
public:
	Courier();
	virtual ~Courier();

	void Init(const std::string& path);
	void AsyncAlarm(const std::string& msg_text);

private:
	void Alarm(std::string msg_text);

private:
	std::string courier_path_;
};

#endif /* SPORE_COURIER_H_ */

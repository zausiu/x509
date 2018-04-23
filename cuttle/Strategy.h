/*
 * Strategy.h
 *
 *  Created on: Apr 22, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef CUTTLE_STRATEGY_H_
#define CUTTLE_STRATEGY_H_

class Strategy
{
public:
	Strategy();
	virtual ~Strategy();

	bool is_allowed(const char* client_ip, int serial)
	{
		return true;
	}
};

#endif /* CUTTLE_STRATEGY_H_ */

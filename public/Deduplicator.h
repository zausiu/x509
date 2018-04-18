/*
 * Deduplicator.h
 *
 *  Created on: Dec 11, 2015
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef PUBLIC_DEDUPLICATOR_H_
#define PUBLIC_DEDUPLICATOR_H_

#include <set>

template<typename T> class Deduplicator
{
public:
	Deduplicator(){};
	virtual ~Deduplicator(){};

	bool Add(T t)
	{
		auto pr = unique_.insert(t);
		return pr.second;
	}

private:
	std::set<T> unique_;
};

#endif /* PUBLIC_DEDUPLICATOR_H_ */

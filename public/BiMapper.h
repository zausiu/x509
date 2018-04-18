/*
 * BiMapper.h
 *
 *  Created on: Jan 20, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef PUBLIC_BIMAPPER_H_
#define PUBLIC_BIMAPPER_H_

#include <exception>
#include <vector>
#include <boost/bimap.hpp>
#include <boost/format.hpp>

template<typename LeftKey, typename RightKey> class BiMapper
{
public:
	typedef boost::bimap<LeftKey, RightKey> BiMap;

	BiMapper()
	{
	}

	void Insert(const LeftKey& left, const RightKey& right)
	{
		m_.insert(typename BiMap::value_type(left, right));
	}

	const RightKey& operator[](const LeftKey& left)
	{
		typename BiMap::left_const_iterator ite = m_.left.find(left);
		if (ite != m_.left.end())
		{
			return ite->second;
		}
		std::string err_info = boost::str(boost::format("Encountered a invalid left key"));
		throw std::runtime_error(err_info);
	}

	const LeftKey& operator[](const RightKey& right)
	{
		typename BiMap::right_const_iterator ite = m_.right.find(right);
		if (ite != m_.right.end())
		{
			return ite->second;
		}
		std::string err_info = boost::str(boost::format("Encountered a invalid right key"));
		throw std::runtime_error(err_info);
	}

	bool IsRightKeyExisted(const RightKey& right)
	{
		typename BiMap::right_const_iterator ite = m_.right.find(right);
		if (ite != m_.right.end())
		{
			return true;
		}

		return false;
	}

	std::vector<LeftKey> GetAllLeftKeys()
	{
		std::vector<LeftKey> vec;
		for (auto ite = m_.left.begin(); ite != m_.left.end(); ++ite)
		{
			vec.push_back(ite->first);
		}

		return vec;
	}

	std::vector<RightKey> GetAllRightKeys()
	{
		std::vector<RightKey> vec;
		for (auto ite = m_.right.begin(); ite != m_.right.end(); ++ite)
		{
			vec.push_back(ite->first);
		}

		return vec;
	}

private:
	BiMap m_;
};

#endif /* PUBLIC_BIMAPPER_H_ */

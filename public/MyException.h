/********************************************************************
 * MyException.h
 * Author: kamuszhou@tencent.com, kamuszhou@qq.com
 * Website: v.qq.com www.dogeye.net
 * Created on: 2014/05/20 14:54
 * Purpose:	An exception with a constructor accepting a string.
*********************************************************************/
#include <boost/exception/all.hpp> 
#include <boost/lexical_cast.hpp> 
#include <exception> 
#include <string> 

/**
 * used to append additional infomation.
 */
typedef boost::error_info<struct tag_errmsg, std::string> ErrmsgInfo; 

/**
 * utilize BOOST_THROW_EXCEPTION to throw MyException.
 */
class MyException:
	public boost::exception, 
	public std::exception 
{ 
public: 
	MyException(const std::string& hint)
		: what_(hint) 
	{ 
	} 

	virtual const char* what() const throw() 
	{ 
		return what_.c_str(); 
	} 

	~MyException()throw()
	{}

private: 
	std::string what_; 
}; 
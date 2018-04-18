/*********************************************
 * proactor.cpp
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  blog.ykyi.net
 * Created on: 28 Mar, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include "MyException.h"
#include "Proactor.h"

Proactor g_proactor;

Proactor::Proactor() : work_(ios_)
{
}

Proactor::~Proactor()
{
	ios_.stop();
	threads_.join_all();
}

void Proactor::AsyncRun(int thrd_num)
{
	if (thrd_num < 0)
	{
		threads_count_ = boost::thread::hardware_concurrency();
	}
	else
	{
        threads_count_ = thrd_num;
	}

	for (int i = 0; i < threads_count_; i++)
	{
		threads_.create_thread(boost::bind(&boost::asio::io_service::run, &ios_));
	}
}

void Proactor::SyncRun(int thrd_num)
{
	AsyncRun(thrd_num);
	ios_.run();
}

void Proactor::Listen(const char* ip, uint16_t port, AcceptorHandler handler)
{
	// Failed to compile using old compiler. I'm forced to use the verbose grammar.
	// using boost::asio::ip::tcp;

	if (listeners_.find(port) != listeners_.end())
	{
		std::ostringstream ss;
		ss << port << " has already been occupied.\n";
		BOOST_THROW_EXCEPTION(MyException(ss.str()));
	}

	// construct an acceptor opened on the given endpoint.
	//boost::asio::ip::tcp::endpoint any_ep(boost::asio::ip::tcp::v4(), port);
	boost::asio::ip::address addr = boost::asio::ip::address::from_string(ip);
	boost::asio::ip::tcp::endpoint ep(addr, port);
	boost::shared_ptr<TheAcceptor> a = boost::make_shared<TheAcceptor>(
			boost::ref(ios_), boost::ref(ep)
	);

	// start the async accept.
	boost::shared_ptr<boost::asio::ip::tcp::socket> s = boost::make_shared<boost::asio::ip::tcp::socket>(boost::ref(ios_));
	a->async_accept(*s, boost::bind(&Proactor::HandleAcceptProxy,
			this, a, s, handler, boost::asio::placeholders::error));

	listeners_[port] = a;
}

SharedStrand Proactor::createStrand()
{
	auto strand_ptr = boost::make_shared<boost::asio::strand>(ios_);

	return strand_ptr;
}

void Proactor::HandleAcceptProxy(boost::shared_ptr<boost::asio::ip::tcp::acceptor> a,
			boost::shared_ptr<boost::asio::ip::tcp::socket> s,
			AcceptorHandler h,
			const boost::system::error_code& error)
{
	boost::shared_ptr<boost::asio::ip::tcp::socket> socket = boost::make_shared<boost::asio::ip::tcp::socket>(boost::ref(ios_));
	a->async_accept(*socket, boost::bind(&Proactor::HandleAcceptProxy,
			this, a, socket, h, boost::asio::placeholders::error));

	// call the user specified callback
	h(s, error);
}

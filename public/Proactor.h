/*********************************************
 * proactor.h
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: 28 Mar, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#ifndef _PROACTOR_H_
#define _PROACTOR_H_

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

class Proactor;
extern Proactor g_proactor;

typedef boost::shared_ptr<boost::asio::strand> SharedStrand;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SharedSocket;
typedef boost::function<void (SharedSocket, const boost::system::error_code& error)> AcceptorHandler;

class Proactor : public boost::noncopyable
{
public:
	Proactor();
	~Proactor();

	void AsyncRun(int thrd_num);
	void SyncRun(int thrd_num);

	/**
	 * open a tcp listening port on all the interface.
	 * @param ip the listening ip
	 * @param port the port which is listened on.
	 * @param handler called when the accept() system call finished.
	 */
	void Listen(const char* ip, uint16_t port, AcceptorHandler handler);

	boost::asio::io_service& GetIOService()
	{
		return ios_;
	}

	int GetCreatedThreadsCount()
	{
		return threads_count_;
	}

	SharedStrand createStrand();

private:
	void HandleAcceptProxy(boost::shared_ptr<boost::asio::ip::tcp::acceptor> a,
			boost::shared_ptr<boost::asio::ip::tcp::socket> s,
			AcceptorHandler h,
			const boost::system::error_code& error);

private:
	boost::asio::io_service ios_;
	boost::asio::io_service::work work_;

	int threads_count_;
	boost::thread_group threads_;

	typedef boost::asio::ip::tcp::acceptor TheAcceptor;
	std::map<uint16_t, boost::shared_ptr<boost::asio::ip::tcp::acceptor> > listeners_;
};

#endif /* _PROACTOR_H_ */

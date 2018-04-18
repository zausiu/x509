/********************************************************************
 * ConsoleOnTCP.h
 * Author: kamuszhou@tencent.com, kamuszhou@qq.com
 * Website: v.qq.com www.dogeye.net
 * Created on: 2014/08/18 14:00
 * Purpose:	Debug Console
*********************************************************************/
#ifndef _CONSOLEONTCP_H_
#define _CONSOLEONTCP_H_

#include <string>
#include <boost/unordered_map.hpp>
#include "Proactor.h"

typedef boost::function<std::string (const std::string& cmdline)> Cannon;

class ConsoleOnTCP : public boost::noncopyable
{
public:
	ConsoleOnTCP();

	/**
	 * @param port Listening on this port
	 */
	void Init(boost::asio::io_service& ios, const char* bound_ip, unsigned short port);
	void RegisterCannon(const char* cmd, Cannon cannon);

	~ConsoleOnTCP(void);

private:
	struct TcpConnection
	{
		TcpConnection(boost::asio::io_service* ios):socket(*ios){}

		boost::asio::ip::tcp::socket socket;
		boost::asio::streambuf rbuff;
		boost::asio::streambuf wbuff;
	};
	void LaunchAccept();
	void HandleAccept(boost::shared_ptr<TcpConnection> conn,
					const boost::system::error_code& error);

	void HandleCommand(boost::shared_ptr<TcpConnection> conn,
					const boost::system::error_code& error, size_t sz);

	void HandleWrite(boost::shared_ptr<TcpConnection> conn,
					const boost::system::error_code& error, size_t sz);

	void PrintHail(boost::asio::ip::tcp::socket& socket);
	void PrintPrompt(boost::asio::ip::tcp::socket& socket);

	std::string GetCmd(const std::string& cmdline);

	void Clear();

private:
	boost::asio::io_service* ios_ptr_;
	boost::asio::ip::tcp::socket* listening_socket_;
	boost::asio::streambuf buf_;

	boost::asio::ip::tcp::acceptor* acceptor_;

	boost::unordered_map<std::string, Cannon> artillery_;
};

#endif

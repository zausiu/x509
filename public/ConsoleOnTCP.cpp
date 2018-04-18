/********************************************************************
 * ConsoleOnTCP.cpp
 * Author: kamuszhou@tencent.com, kamuszhou@qq.com
 * Website: v.qq.com blog.ykyi.net
 * Created on: 2014/08/18 14:00
 * Purpose:	Debug Console
*********************************************************************/

#include <boost/make_shared.hpp>
#include "ConsoleOnTCP.h"
#include "seclog/log.h"

ConsoleOnTCP::ConsoleOnTCP()
{
	listening_socket_ = NULL;
	acceptor_ = NULL;
}

ConsoleOnTCP::~ConsoleOnTCP(void)
{
	Clear();
}

void ConsoleOnTCP::Init(boost::asio::io_service& ios, const char* bound_ip, unsigned short port)
{
	Clear();

	if (port == 0)
	{
//		LG_INFO("debug console port is 0, quit creating debug console");
		return;
	}

	boost::asio::ip::address addr = boost::asio::ip::address_v4::from_string(bound_ip);
	boost::asio::ip::tcp::endpoint ep(addr, port);

	ios_ptr_ = &ios;
	listening_socket_ = new boost::asio::ip::tcp::socket(ios);
	acceptor_ = new boost::asio::ip::tcp::acceptor(ios, ep);

	LaunchAccept();
}

void ConsoleOnTCP::RegisterCannon(const char* cmd, Cannon cannon)
{
	artillery_[cmd] = cannon;
}

void ConsoleOnTCP::LaunchAccept()
{
	boost::shared_ptr<TcpConnection> conn = boost::make_shared<TcpConnection>(ios_ptr_);
	acceptor_->async_accept(conn->socket,
			boost::bind(&ConsoleOnTCP::HandleAccept, this, conn, boost::asio::placeholders::error)
		);
}

void ConsoleOnTCP::HandleAccept(boost::shared_ptr<TcpConnection> conn,
					const boost::system::error_code& error)
{
	PrintHail(conn->socket);
	PrintPrompt(conn->socket);
	async_read_until(conn->socket, conn->rbuff, '\n',
		boost::bind(&ConsoleOnTCP::HandleCommand, this, conn,
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred
		)
	);

	LaunchAccept();
}

void ConsoleOnTCP::HandleCommand(boost::shared_ptr<TcpConnection> conn,
					const boost::system::error_code& error, std::size_t sz)
{
	if (error)
	{
		std::cerr << error.message() << std::endl;
		return;
	}

	std::istream s(&conn->rbuff);
	std::string cmdline;

	std::getline(s, cmdline);

	std::string cmd = GetCmd(cmdline);

	if (cmd == "quit" || cmd == "exit" || cmd == "bye")
	{
		return;
	}

	std::string result;
	auto ite = artillery_.find(cmd);
	if (ite != artillery_.end())
	{
		result = (ite->second)(cmdline);
	}
	else
	{
		result = "Unrecognized command.";
	}

	std::ostream os(&conn->wbuff);
	os << result << std::endl;
	//conn->wbuff.commit(100*1000);

	async_write(conn->socket, conn->wbuff,
				boost::bind(&ConsoleOnTCP::HandleWrite, this, conn,
					boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
	);
}
	
void ConsoleOnTCP::HandleWrite(boost::shared_ptr<TcpConnection> conn,
					const boost::system::error_code& error, size_t sz)
{
	if (error)
	{
		std::cout << error.message() << std::endl;
	}

	PrintPrompt(conn->socket);
	async_read_until(conn->socket, conn->rbuff, '\n',
		boost::bind(&ConsoleOnTCP::HandleCommand, this, conn,
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred
		)
	);
}

void ConsoleOnTCP::PrintHail(boost::asio::ip::tcp::socket& socket)
{
	socket.write_some(boost::asio::buffer("Welcome to Debug Console. kamuszhou@tencent.com\n"));
}

void ConsoleOnTCP::PrintPrompt(boost::asio::ip::tcp::socket& socket)
{
	socket.write_some(boost::asio::buffer("Console>"));
}

std::string ConsoleOnTCP::GetCmd(const std::string& cmdline)
{
	std::istringstream iss(cmdline);
	std::string cmd;
	iss >> cmd;

	return cmd;
}

void ConsoleOnTCP::Clear()
{
	delete listening_socket_;
	delete acceptor_;

	listening_socket_ = NULL;
	acceptor_ = NULL;
	artillery_.clear();
}

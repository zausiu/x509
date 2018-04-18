/*
 * DBBroker.cpp
 *
 *  Created on: Jan 19, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wblog.ykyi.net 
 */
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
#include <TinyDBBroker.h>
#include "seclog/log.h"

TinyDBBroker::TinyDBBroker()
{
	driver_ = NULL;
}

TinyDBBroker::~TinyDBBroker()
{
	BOOST_FOREACH(sql::Connection* conn, db_conns_)
	{
		conn->close();
		delete conn;
	}
	db_conns_.clear();
}

void TinyDBBroker::Init()
{
    driver_ = sql::mysql::get_mysql_driver_instance();
}

sql::Connection* TinyDBBroker::AddConn(const char* ip, uint16_t port, const char* user,
						   const char* password, const char* schema)
{
    sql::ConnectOptionsMap conn_properties;
    conn_properties["hostName"] = std::string(ip);
    conn_properties["port"] = port;
    conn_properties["userName"] = std::string(user);
    conn_properties["password"] = std::string(password);
    conn_properties["OPT_RECONNECT"] = true;
    if (schema != NULL)
    {
    	conn_properties["schema"] = std::string(schema);
    }

//    LG_INFO("Begin to connect to mysql server on %s:%hu", ip, port);
    sql::Connection* conn = driver_->connect(conn_properties);
//    LG_INFO("Connection has been established.");

    //bool my_ture = true;
    //task_db_conn_->setClientOption("OPT_RECONNECT", &my_ture);

    boost::scoped_ptr<sql::Statement> statement(conn->createStatement());
    statement->execute("set names utf8");

    db_conns_.insert(conn);

    return conn;
}

void TinyDBBroker::CloseConn(sql::Connection* conn)
{
	auto ite = db_conns_.find(conn);
	assert(ite != db_conns_.end());
	conn->close();
	delete conn;
	db_conns_.erase(ite);
}

void TinyDBBroker::MakeSureSchemaExistAndUseIt(sql::Connection* conn, const char* schema)
{
	boost::scoped_ptr<sql::Statement> statement(conn->createStatement());

//	LG_INFO("created a database: %s", schema);

	std::ostringstream oss;
	oss << "create database if not exists " << schema << " character set utf8 collate utf8_general_ci";
	statement->execute(oss.str());
	conn->setSchema(schema);
}

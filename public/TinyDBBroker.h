/*
 * TinyDBBroker.h
 *
 *  Created on: Jan 19, 2016
 *  Author: kamuszhou@tencent.com
 *  website: wwww.dogeye.net 
 */

#ifndef SPORE_TINYDBBROKER_H_
#define SPORE_TINYDBBROKER_H_

#include <set>
#include <boost/shared_ptr.hpp>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_error.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>
#include <cppconn/driver.h>

typedef boost::shared_ptr<sql::PreparedStatement> SharedPtrPreparedStatement;

class TinyDBBroker
{
public:
	TinyDBBroker();
	virtual ~TinyDBBroker();

	void Init();

	// the returned connection is owned by TinyDBBroker and be taken care of its lifetime
	sql::Connection* AddConn(const char* ip, uint16_t port, const char* user,
					 const char* password, const char* schema);
	void CloseConn(sql::Connection* conn);
	void MakeSureSchemaExistAndUseIt(sql::Connection* conn, const char* schema);

protected:
    std::set<sql::Connection*> db_conns_;
    sql::mysql::MySQL_Driver* driver_;
};

#endif /* SPORE_TINYDBBROKER_H_ */

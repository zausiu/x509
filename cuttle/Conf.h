/*
 * Conf.h
 *
 *  Created on: Mar 22, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef LOBSTER_CONF_H_
#define LOBSTER_CONF_H_

#include <string>
#include <vector>
#include "spdlog/spdlog.h"

class Conf;
extern Conf g_conf;

class Conf
{
public:
	Conf();
	virtual ~Conf();

	void init(int argc, char** argv);

	unsigned short getListeningPort()const
	{
		return listening_port_;
	}

	bool needCopyLogToConsole()const
	{
		return copy_log_to_console_;
	}

	const std::string& getLogFileName()const
	{
		return log_file_name_;
	}

	spdlog::level::level_enum getLogLevel()const
	{
		return log_level_;
	}

    const std::string& getCACrtPath()const
    {
        return ca_crt_path_;
    }

private:
	void loadConfFromJson();

private:
	std::string conf_json_path_;

	unsigned short listening_port_;

	bool copy_log_to_console_;
	std::string log_file_name_;
	std::string log_level_str_;
	spdlog::level::level_enum log_level_;

    std::string ca_crt_path_;
};

#endif

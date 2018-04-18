/*
 * SnazzyLogger.h
 *
 *  Created on: Jan 5, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef LOBSTER_SNAZZYLOGGER_H_
#define LOBSTER_SNAZZYLOGGER_H_

#include <string>
#include "spdlog/spdlog.h"

#define LOGGER_NAME "snazzy"
extern std::shared_ptr<spdlog::logger> LOG;

void InitSpdLogger(const std::string& log_file_path, bool copy_log_to_console,
		spdlog::level::level_enum log_level);

#endif /* LOBSTER_SNAZZYLOGGER_H_ */

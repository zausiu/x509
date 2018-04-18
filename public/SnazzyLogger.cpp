/*
 * SnazzyLogger.cpp
 *
 *  Created on: Jan 5, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#include <vector>
#include <iostream>
#include "SnazzyLogger.h"

std::shared_ptr<spdlog::logger> LOG;

void InitSpdLogger(const std::string& log_file_path, bool copy_log_to_console,
		spdlog::level::level_enum log_level)
{
	spdlog::set_async_mode(1024, spdlog::async_overflow_policy::block_retry);
	std::vector<spdlog::sink_ptr> sinks;
	if (copy_log_to_console)
	{
		sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
	}
	// sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_file_path, 23, 59));
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file_path, 10 * 1024 * 1024, 30));
	auto combined_logger = std::make_shared<spdlog::async_logger>(LOGGER_NAME, std::begin(sinks), std::end(sinks), 1024);
	//register it if you need to access it globally
	combined_logger->set_pattern("[%c][thread %t][%l] %v");

//	combined_logger->set_level(spdlog::level::trace);
	combined_logger->set_level(log_level);
	combined_logger->flush_on(spdlog::level::warn);
	spdlog::register_logger(combined_logger);
	// spdlog::set_pattern("[%c][%l] %v");

	LOG = combined_logger;

	// for test
//	LOG->info("filadfjldifhoad8fyqw98ydhl");
}

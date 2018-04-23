/*
 * Conf.cpp
 *
 *  Created on: Mar 22, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#include <iostream>
#include <fstream>
#include <getopt.h>
#include "Conf.h"
#include "json/json.h"
#include "spdlog/formatter.h"

Conf g_conf;

Conf::Conf()
{
}

Conf::~Conf()
{
}

void Conf::init(int argc, char** argv)
{
	conf_json_path_ = "../cuttle/conf.json";

	int option_index;
	struct option long_options[] = {
//			{"rule-file", required_argument, NULL, 'f'},
//			{"max-load-factor", required_argument, NULL, 'l'},
//			{"check-duplicated-rules", no_argument, NULL, 'C'},
//			{"magic", required_argument, NULL,  'm' },
			{"conf", required_argument, NULL, 'c'},
			{"version", no_argument, NULL,  'v' },
			{0, 0, 0, 0}
	};
	while (true)
	{
//		int ch = getopt_long(argc, argv, "Cf:l:m:v", long_options, &option_index);
		int ch = getopt_long(argc, argv, "c:v", long_options, &option_index);
		if (ch == -1)
		{
			break;
		}

		switch (ch)
		{
//		case 'f':
//			rule_file_path_ = optarg;
//			break;
//
//		case 'l':
//			max_load_factor_ = std::stod(optarg);
//			break;
//
//		case 'm':
//			magic_ = optarg;
//			break;
//
//		case 'C':
//			check_duplicated_rules_ = true;
//			break;

		case 'c':
			conf_json_path_ = optarg;
			break;

		case 'v':
			std::cout << "VERSION " << VERSION_MAJOR << "." << VERSION_MINOR << " Mar 2018\n";
			std::cout << "Lobster is a server that to the cryptography things.\n";
			std::cout << "written by kamuszhou@tencent.com http//blog.ykyi.net\n";
			std::cout << "Copyright(C) by Tencent\n";
			exit(0);
		}
	}

	if (optind < argc)
	{
		std::cerr << "unrecognized ARGV-elements: ";
		while (optind < argc)
		{
			std::cerr << argv[optind++] << " ";
		}
		std:: cerr << std::endl;
	}

	std::cout << "Loading config file: " << conf_json_path_ << "\n";
	loadConfFromJson();

	std::cout << "log_level: " << log_level_str_ << "\n";

	std::cout << "...\n" << "Conf-loading's DONE.\n";
}

void Conf::loadConfFromJson()
{
	std::ifstream ifs(conf_json_path_);

	Json::Reader reader;
	Json::Value jv_root;
	bool ok = reader.parse(ifs, jv_root, false);
	if (!ok)
	{
		const std::string& wrong = fmt::format("Failed to open json file: [{}].", conf_json_path_);
		throw std::runtime_error(wrong);
	}

	Json::Value default_listening_port(8333);
	listening_port_ = jv_root.get("listening_port", default_listening_port).asUInt();

	Json::Value default_copy_log_to_console = true;
	copy_log_to_console_ = jv_root.get("copy_log_to_console", default_copy_log_to_console).asBool();

	Json::Value default_log_file_name("cuttle");
	log_file_name_ = jv_root.get("log_file_name", default_log_file_name).asString();

	Json::Value default_log_level("debug");
	log_level_str_ = jv_root.get("log_level", default_log_level).asString();
	std::map<std::string, spdlog::level::level_enum> level_map{
		{"trace", spdlog::level::trace},
		{"debug", spdlog::level::debug},
		{"info", spdlog::level::info},
		{"warn", spdlog::level::warn},
		{"err", spdlog::level::err},
		{"critical", spdlog::level::critical},
		{"off", spdlog::level::off},
	};
	log_level_ = level_map.at(log_level_str_);

    ca_crt_path_ = jv_root["ca_crt_path"].asString();
}

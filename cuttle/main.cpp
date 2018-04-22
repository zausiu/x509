/////////////////////////////////////////////////////////////
/// written by kamuszhou@tencent.com http://blog.ykyi.net
/// created on April 18, 2018
/////////////////////////////////////////////////////////////
#include "CuttleImpl.h"
#include "Conf.h"
#include "SnazzyLogger.h"

int main(int argc, char** argv)
{
	g_conf.init(argc, argv);

	const std::string& log_file_name = g_conf.getLogFileName();
	spdlog::level::level_enum log_level = g_conf.getLogLevel();
	bool copy_log_to_console = g_conf.needCopyLogToConsole();
	InitSpdLogger(log_file_name, copy_log_to_console, log_level);

    unsigned short port = g_conf.getListeningPort();
    const std::string& ca_crt_path = g_conf.getCACrtPath();
    CuttleImpl cuttle;
    cuttle.init(port, ca_crt_path.c_str());
    cuttle.run();

	return 0;
}

/*
 * NeatUtilities.cpp
 *
 *  Created on: Apr 20, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <iostream>
#include "NeatUtilities.h"
#include "spdlog/fmt/fmt.h"

std::string form_dayhour_uid(int year, int month, int day, int hour)
{
        std::string uid = fmt::format("{0:04}-{1:02}-{2:02}:{3:02}", year, month, day, hour);

        return uid;
}

int send_all(int fd, const char* m, int m_len)
{
        int ret;
        int remained_len = m_len;

        while (remained_len > 0)
        {
                const char* pos = m + m_len - remained_len;
                ret = send(fd, pos, remained_len, MSG_NOSIGNAL);
                if (ret < 0)
                {
                        std::string err_msg = fmt::format("Failed to send message, cos: {}", strerror(errno));
                        std::cerr << err_msg << std::endl;
                        return ret;
                }
                else
                {
                	remained_len -= ret;
                }
        }

        return 0;
}


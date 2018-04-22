/*
 * NeatUtilities.h
 *
 *  Created on: Apr 20, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef PUBLIC_NEATUTILITIES_H_
#define PUBLIC_NEATUTILITIES_H_

#include <string>

std::string form_dayhour_uid(int year, int month, int day, int hour);

int send_all(int fd, const char* m, int m_len);

#endif /* PUBLIC_NEATUTILITIES_H_ */

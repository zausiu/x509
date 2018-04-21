/*
 * OtterImpl.cpp
 *
 *  Created on: Apr 19, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#include "OtterImpl.h"

OtterImpl::OtterImpl()
{
}

OtterImpl::~OtterImpl()
{
}

void OtterImpl::init(const char* crt_path)
{
	x509_crt_.init(crt_path);
}

int OtterImpl::get_key_and_salt(int year, int month, int day, int hour,
			char* key, char* salt)
{
	return 0;
}

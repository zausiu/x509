/*
 * OtterImpl.h
 *
 *  Created on: Apr 19, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef OTTER_OTTERIMPL_H_
#define OTTER_OTTERIMPL_H_

#include "x509/X509Crt.h"

class OtterImpl
{
public:
	OtterImpl();
	virtual ~OtterImpl();

	void init(const char* crt_path);

	int get_key_and_salt(int year, int month, int day, int hour,
			char* key, char* salt);

private:
	X509Crt x509_crt_;
};

#endif /* OTTER_OTTERIMPL_H_ */

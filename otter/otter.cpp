/////////////////////////////////////////////////////////////
/// written by kamuszhou@tencent.com http://blog.ykyi.net
/// created on April 18, 2018
/////////////////////////////////////////////////////////////
#include "otter.h"
#include "OtterImpl.h"

OtterImpl otter_impl;

void otter_init(const char* crt_path, const char* key_path)
{
	otter_impl.init(crt_path, key_path);
}

int otter_get_key_and_salt(int year, int month, int day, int hour,
		char* key, char* salt)
{
	return otter_impl.get_key_and_salt(year, month, day, hour, key, salt);
}

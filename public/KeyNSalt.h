//////////////////////////////////////
/// http://blog.ykyi.net/
////////////////////////////////////////////////

#pragma once

#include <string.h>

struct KeyNSalt
{
	KeyNSalt(){}
	KeyNSalt(const char* k, const char* s)
	{
		memcpy(key, k, 32);
		memcpy(salt, s, 32);
		memset(pad0, 0, 32);
		memset(pad1, 1, 32);
		memset(pad2, 2, 32);
	}
	~KeyNSalt(){}

	char pad0[32];
	char key[32];
	char pad1[32];
	char salt[32];
	char pad2[32];
};

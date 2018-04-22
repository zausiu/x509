/////////////////////////////////////////////////////////////
/// written by kamuszhou@tencent.com http://blog.ykyi.net
/// created on April 18, 2018
/////////////////////////////////////////////////////////////

extern "C" void otter_init(const char* crt_path, const char* key_path);

// return 0 on success, otherwise non-zero is returned
extern "C" int otter_get_key_and_salt(int year, int month, int day, int hour,
		char* key, char* salt);

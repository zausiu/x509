/*
 * KamusCrypto.cpp
 *
 *  Created on: Mar 30, 2016
 *  Author: kamuszhou@tencent.com
 *  website: blog.ykyi.net
 */
#include <KamusCrypto.h>
#include <exception>

KamusCrypto::KamusCrypto()
{
	memset(nonce_, '\xab', sizeof(nonce_));
	initiated_ = false;
}

KamusCrypto::~KamusCrypto()
{
	initiated_ = false;
}

void KamusCrypto::Init(bool strict)
{
	// 0 on success, -1 on failure and 1 if the library had already been initialized
	int ret = sodium_init();
	assert(ret != -1);
	if (crypto_aead_aes256gcm_is_available() == 0)
	{
		throw std::runtime_error("No hardware support for GCM");
	}
	initiated_ = true;
	strict_ = strict;
}

void KamusCrypto::HandleErrors(void)
{
	abort();
}

int KamusCrypto::Encrypt(const unsigned char *plaintext, uint64_t plaintext_len,
		unsigned char *ciphertext, const unsigned char* key)
{
	return Encrypt(plaintext, plaintext_len, ciphertext, nonce_, key);
}

int KamusCrypto::Encrypt(const unsigned char *plaintext, uint64_t plaintext_len,
		unsigned char *ciphertext, const unsigned char* nonce, const unsigned char* key)
{
	assert(initiated_);

	unsigned char the_key[crypto_aead_aes256gcm_KEYBYTES];
	memset(the_key, 0xff, sizeof(the_key));
	if (strict_)
	{
		memcpy((char*)the_key, (char*)key, sizeof(the_key));
	}
	else
	{
		strncpy((char*)the_key, (char*)key, sizeof(the_key));
	}

	unsigned long long ciphertext_len;
	int code = crypto_aead_aes256gcm_encrypt(ciphertext, &ciphertext_len,
			plaintext, plaintext_len,
			NULL, 0,
			NULL, nonce, the_key);

	if (code == 0)
	{
		return (int)ciphertext_len;
	}
	else
	{
		return code;
	}
}

int KamusCrypto::Encrypt(const unsigned char *plaintext, uint64_t plaintext_len,
		const unsigned char *salt,  uint64_t salt_len,
		unsigned char *ciphertext, const unsigned char* key)
{
	int total_len = plaintext_len + salt_len;
	std::vector<char> plaintext_with_salt(total_len);
	char* data = plaintext_with_salt.data();
	memcpy(data, salt, salt_len);
	memcpy(data + salt_len, plaintext, plaintext_len);

	return Encrypt((const unsigned char*)data, total_len, ciphertext, key);
}

SharedSmartMem KamusCrypto::EncryptM(const unsigned char *plaintext, uint64_t plaintext_len, const unsigned char* key)
{
	DECLARE_SHARED_SMART_MEM(mem, plaintext_len + 128);
	int len = Encrypt(plaintext, plaintext_len, (unsigned char*)mem->data(), key);
	mem->resize(len);

	return mem;
}

SharedSmartMem KamusCrypto::EncryptM(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char* nonce, const unsigned char* key)
{
	DECLARE_SHARED_SMART_MEM(mem, plaintext_len + 128);
	int len = Encrypt(plaintext, plaintext_len, (unsigned char*)mem->data(), nonce, key);
	mem->resize(len);

	return mem;
}

SharedSmartMem KamusCrypto::EncryptM(const unsigned char *plaintext, uint64_t plaintext_len,
		const unsigned char *salt,  uint64_t salt_len, const unsigned char* key)
{
	DECLARE_SHARED_SMART_MEM(mem, plaintext_len + salt_len + 128);
	int len = Encrypt(plaintext, plaintext_len, salt, salt_len ,(unsigned char*)mem->data(), key);
	mem->resize(len);

	return mem;
}

std::string KamusCrypto::EncryptS(const unsigned char *plaintext, uint64_t plaintext_len, const unsigned char* key)
{
	std::string str;
	str.resize(plaintext_len + 128);
	int len = Encrypt(plaintext, plaintext_len, (unsigned char*)str.data(), key);
	str.resize(len);

	return str;
}

std::string KamusCrypto::EncryptS(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char* nonce, const unsigned char* key)
{
	std::string str;
	str.resize(plaintext_len + 128);
	int len = Encrypt(plaintext, plaintext_len, (unsigned char*)str.data(), nonce, key);
	str.resize(len);

	return str;
}

std::string KamusCrypto::EncryptS(const unsigned char *plaintext, uint64_t plaintext_len,
		const unsigned char *salt,  uint64_t salt_len, const unsigned char* key)
{
	std::string str;
	str.resize(plaintext_len + salt_len + 128);
	int len = Encrypt(plaintext, plaintext_len, salt, salt_len,
			(unsigned char*)str.data(), key);
	str.resize(len);

	return str;
}

std::string KamusCrypto::EncryptBase64(const unsigned char *plaintext, uint64_t plaintext_len,
		const unsigned char* key)
{
	return EncryptBase64(plaintext, plaintext_len, nonce_, key);
}

std::string KamusCrypto::EncryptBase64(const unsigned char *plaintext, uint64_t plaintext_len,
		const unsigned char* nonce, const unsigned char* key)
{
	SharedSmartMem mem = EncryptM(plaintext, plaintext_len, nonce, key);
	int base64_len = Base64encode_len(mem->size());
	std::string base64_str;
	base64_str.resize(base64_len);
	int ret = Base64encode((char*)base64_str.data(), mem->data(), mem->size());
	// return base64_str;  // a subtle BUG arises here
	return base64_str.c_str();  // so the trailing NULL will be removed from std::string
}

std::string KamusCrypto::EncryptBase64(const unsigned char *plaintext, uint64_t plaintext_len,
		const unsigned char *salt,  uint64_t salt_len, const unsigned char* key)
{
	SharedSmartMem mem = EncryptM(plaintext, plaintext_len, salt, salt_len, key);
	int base64_len = Base64encode_len(mem->size());
	std::string base64_str;
	base64_str.resize(base64_len);
	int ret = Base64encode((char*)base64_str.data(), mem->data(), mem->size());
	// return base64_str;  // a subtle BUG arises here
	return base64_str.c_str();  // so the trailing NULL will be removed from std::string
}

int KamusCrypto::Decrypt(const unsigned char *ciphertext, uint64_t ciphertext_len,
		unsigned char *plaintext, const unsigned char* key)
{
	return Decrypt(ciphertext, ciphertext_len, plaintext, nonce_, key);
}

int KamusCrypto::Decrypt(const unsigned char *ciphertext, uint64_t ciphertext_len, unsigned char *plaintext,
			const unsigned char* nonce, const unsigned char* key)
{
	assert(initiated_);

	unsigned char the_key[crypto_aead_aes256gcm_KEYBYTES];
	memset(the_key, 0xff, sizeof(the_key));
	strncpy((char*)the_key, (char*)key, sizeof(the_key));

	unsigned long long plaintext_len;
	if (ciphertext_len < crypto_aead_aes256gcm_ABYTES ||
			crypto_aead_aes256gcm_decrypt(plaintext, &plaintext_len,
					NULL,
					ciphertext, ciphertext_len,
					NULL,
					0,
					nonce, the_key) != 0)
	{
		return -1;
	}
	else
	{
		return (int)plaintext_len;
	}
}

int KamusCrypto::Decrypt(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* salt, int salt_len,
			unsigned char *plaintext, const unsigned char* key)
{
	SharedSmartMem plaintext_with_salt = DecryptM(ciphertext, ciphertext_len, key);
	int total_len = plaintext_with_salt->size();
	char* data = plaintext_with_salt->data();
	if (salt_len > total_len)
	{
		return -2;
	}
	bool matched = (memcmp(data, salt, salt_len) == 0);
	if (!matched)
	{
		return -1;
	}
	memcpy(plaintext, data + salt_len, total_len - salt_len);

	return total_len - salt_len;
}

SharedSmartMem KamusCrypto::DecryptM(const unsigned char *ciphertext, uint64_t ciphertext_len,
		const unsigned char* key)
{
	DECLARE_SHARED_SMART_MEM(mem, ciphertext_len);
	int len = Decrypt(ciphertext, ciphertext_len, (unsigned char*)mem->data(), key);
	assert(len >= 0);
	mem->resize(len);

	return mem;
}

SharedSmartMem KamusCrypto::DecryptM(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* nonce, const unsigned char* key)
{
	DECLARE_SHARED_SMART_MEM(mem, ciphertext_len);
	int len = Decrypt(ciphertext, ciphertext_len, (unsigned char*)mem->data(), nonce, key);
	assert(len >= 0);
	mem->resize(len);

	return mem;
}

SharedSmartMem KamusCrypto::DecryptM(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* salt, int salt_len, const unsigned char* key)
{
	DECLARE_SHARED_SMART_MEM(mem, ciphertext_len);
	int len = Decrypt(ciphertext, ciphertext_len, salt, salt_len,
			(unsigned char*)mem->data(), key);
	assert(len >= 0);
	mem->resize(len);

	return mem;
}

std::string KamusCrypto::DecryptS(const unsigned char *ciphertext, uint64_t ciphertext_len,
		const unsigned char* key)
{
	std::string str;
	str.resize(ciphertext_len);
	int len = Decrypt(ciphertext, ciphertext_len, (unsigned char*)str.data(), key);
	str.resize(len);

	return str;
}

std::string KamusCrypto::DecryptS(const unsigned char *ciphertext, uint64_t ciphertext_len,
		const unsigned char* nonce, const unsigned char* key)
{
	std::string str;
	str.resize(ciphertext_len);
	int len = Decrypt(ciphertext, ciphertext_len, (unsigned char*)str.data(), nonce, key);
	assert(len >= 0);
	str.resize(len);

	return str;
}

std::string KamusCrypto::DecryptS(const unsigned char *ciphertext, uint64_t ciphertext_len,
		const unsigned char* salt, int salt_len, const unsigned char* key)
{
	std::string str;
	str.resize(ciphertext_len);
	int len = Decrypt(ciphertext, ciphertext_len, salt, salt_len,
			(unsigned char*)str.data(), key);
	assert(len >= 0);
	str.resize(len);

	return str;
}

int KamusCrypto::DecryptBase64(const char* base64_str, unsigned char* plaintext, const unsigned char* key)
{
	int len = Base64decode_len(base64_str);
	DECLARE_SHARED_SMART_MEM(mem, len);
	int len2 = Base64decode(mem->data(), base64_str);
	int plain_len = Decrypt((unsigned char*)mem->data(), len2, plaintext, key);

	return plain_len;
}

int KamusCrypto::DecryptBase64(const char* base64_str, unsigned char* plaintext,
			const unsigned char* nonce, const unsigned char* key)
{
	int len = Base64decode_len(base64_str);
	DECLARE_SHARED_SMART_MEM(mem, len);
	int len2 = Base64decode(mem->data(), base64_str);
	int plain_len = Decrypt((unsigned char*)mem->data(), len2, plaintext, nonce, key);

	return plain_len;
}

int KamusCrypto::DecryptBase64(const char* base64_str, unsigned char* plaintext,
			const unsigned char* salt, int salt_len, const unsigned char* key)
{
	int len = Base64decode_len(base64_str);
	DECLARE_SHARED_SMART_MEM(mem, len);
	int len2 = Base64decode(mem->data(), base64_str);
	int plain_len = Decrypt((unsigned char*)mem->data(), len2,
			salt, salt_len, plaintext, key);

	return plain_len;
}

std::string KamusCrypto::DecryptBase64S(const char* base64_str, const unsigned char* key)
{
	return DecryptBase64S(base64_str, nonce_, key);
}

std::string KamusCrypto::DecryptBase64S(const char* base64_str, const unsigned char* nonce,
			const unsigned char* key)
{
	int base64s_len = strlen(base64_str);
	if (base64s_len == 0)
	{
		return "";
	}
	DECLARE_SHARED_SMART_MEM(mem, base64s_len);
	char* data = mem->data();
	int ret = DecryptBase64(base64_str, (unsigned char*)mem->data(), nonce, key);
	if (ret > 0)
	{
		return std::string(data, data + ret);
	}
	else
	{
		abort();
	}

}

std::string KamusCrypto::DecryptBase64S(const char* base64_str, const unsigned char* salt, int salt_len,
		const unsigned char* key)
{
	int base64s_len = strlen(base64_str);
	if (base64s_len < salt_len)
	{
		abort();
	}
	DECLARE_SHARED_SMART_MEM(mem, base64s_len);
	char* data = mem->data();
	int ret = DecryptBase64(base64_str, (unsigned char*)mem->data(),
			salt, salt_len, key);
	if (ret > 0)
	{
		return std::string(data, data + ret);
	}
	else
	{
		abort();
	}

	return mem->data();
}

//
///*
// * Crypto.cpp
// *
// *  Created on: Mar 30, 2016
// *  Author: kamuszhou@tencent.com
// *  website: wblog.ykyi.net
// */
//#include "Crypto.h"
//
////Crypto g_crypto;
//
//Crypto::Crypto()
//{
//}
//
//Crypto::~Crypto()
//{
//}
//
//void Crypto::Init()
//{
//	/* A 128 bit IV */
//	iv_ = (unsigned char *)"8879754598476847";
//	/* Create and initialise the context */
//	if(!(ctx_ = EVP_CIPHER_CTX_new())) HandleErrors();
//	/* Initialise the library */
//	ERR_load_crypto_strings();
//	OpenSSL_add_all_algorithms();
//	OPENSSL_config(NULL);
//}
//
//void Crypto::HandleErrors(void)
//{
//	ERR_print_errors_fp(stderr);
//	abort();
//}
//
//int Crypto::Encrypt(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, const unsigned char* key)
//{
//	unsigned char the_key[32];
//	memset(the_key, 0xff, sizeof(the_key));
//	strncpy((char*)the_key, (char*)key, sizeof(the_key));
//
//	int len;
//	int ciphertext_len;
//
//	/* Initialise the encryption operation. IMPORTANT - ensure you use a key
//	 * and IV size appropriate for your cipher
//	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
//	 * IV size for *most* modes is the same as the block size. For AES this
//	 * is 128 bits */
//	if(1 != EVP_EncryptInit_ex(ctx_, EVP_aes_256_cbc(), NULL, the_key, iv_))
//		HandleErrors();
//
//	/* Provide the message to be encrypted, and obtain the encrypted output.
//	 * EVP_EncryptUpdate can be called multiple times if necessary
//	 */
//	if(1 != EVP_EncryptUpdate(ctx_, ciphertext, &len, plaintext, plaintext_len))
//		HandleErrors();
//	ciphertext_len = len;
//
//	/* Finalise the encryption. Further ciphertext bytes may be written at
//	 * this stage.
//	 */
//	if(1 != EVP_EncryptFinal_ex(ctx_, ciphertext + len, &len)) HandleErrors();
//	ciphertext_len += len;
//
//	return ciphertext_len;
//}
//
//SharedSmartMem Crypto::EncryptM(const unsigned char *plaintext, int plaintext_len, const unsigned char* key)
//{
//	DECLARE_SHARED_SMART_MEM(mem, plaintext_len*3 + 64);
//	int len = Encrypt(plaintext, plaintext_len, (unsigned char*)mem->data(), key);
//	mem->resize(len);
//
//	return mem;
//}
//
//std::string Crypto::EncryptS(const unsigned char *plaintext, int plaintext_len, const unsigned char* key)
//{
//	std::string str;
//	str.resize(plaintext_len*3 + 64);
//	int len = Encrypt(plaintext, plaintext_len, (unsigned char*)str.data(), key);
//	str.resize(len);
//
//	return str;
//}
//
//int Crypto::Decrypt(const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, const unsigned char* key)
//{
//	unsigned char the_key[32];
//	memset(the_key, 0xff, sizeof(the_key));
//	strncpy((char*)the_key, (char*)key, sizeof(the_key));
//	//EVP_CIPHER_CTX *ctx;
//
//	int len;
//	int plaintext_len;
//
//	/* Create and initialise the context */
//	//if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
//
//	/* Initialise the decryption operation. IMPORTANT - ensure you use a key
//	 * and IV size appropriate for your cipher
//	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
//	 * IV size for *most* modes is the same as the block size. For AES this
//	 * is 128 bits */
//	if(1 != EVP_DecryptInit_ex(ctx_, EVP_aes_256_cbc(), NULL, the_key, iv_))
//		HandleErrors();
//
//	/* Provide the message to be decrypted, and obtain the plaintext output.
//	 * EVP_DecryptUpdate can be called multiple times if necessary
//	 */
//	if(1 != EVP_DecryptUpdate(ctx_, plaintext, &len, ciphertext, ciphertext_len))
//		HandleErrors();
//	plaintext_len = len;
//
//	/* Finalise the decryption. Further plaintext bytes may be written at
//	 * this stage.
//	 */
//	if(1 != EVP_DecryptFinal_ex(ctx_, plaintext + len, &len)) HandleErrors();
//	plaintext_len += len;
//
//	/* Clean up */
//	//EVP_CIPHER_CTX_free(ctx);
//
//	return plaintext_len;
//}
//
//SharedSmartMem Crypto::DecryptM(const unsigned char *ciphertext, int ciphertext_len, const unsigned char* key)
//{
//	DECLARE_SHARED_SMART_MEM(mem, ciphertext_len);
//	int len = Decrypt(ciphertext, ciphertext_len, (unsigned char*)mem->data(), key);
//	mem->resize(len);
//
//	return mem;
//}
//
//std::string Crypto::DecryptS(const unsigned char *ciphertext, int ciphertext_len, const unsigned char* key)
//{
//	std::string str;
//	str.resize(ciphertext_len);
//	int len = Decrypt(ciphertext, ciphertext_len, (unsigned char*)str.data(), key);
//	str.resize(len);
//
//	return str;
//}

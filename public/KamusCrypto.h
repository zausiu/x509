/*
 * KamusCrypto.h
 *
 *  Created on: Mar 30, 2016
 *  Author: kamuszhou@tencent.com
 *  website: blog.ykyi.net
 */

#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <sodium.h>
#include "Base64.h"
#include "CommonHelper.h"

class KamusCrypto;
extern KamusCrypto g_crypto;

class KamusCrypto
{
public:
	KamusCrypto();
	virtual ~KamusCrypto();

	void Init(bool strict = false);
	int Encrypt(const unsigned char *plaintext, uint64_t plaintext_len, unsigned char *ciphertext,
			const unsigned char* key);
	int Encrypt(const unsigned char *plaintext, uint64_t plaintext_len, unsigned char *ciphertext,
			const unsigned char* nonce, const unsigned char* key);
	int Encrypt(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char *salt,  uint64_t salt_len,
			unsigned char *ciphertext, const unsigned char* key);

	SharedSmartMem EncryptM(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char* key);
	SharedSmartMem EncryptM(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char* nonce, const unsigned char* key);
	SharedSmartMem EncryptM(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char *salt,  uint64_t salt_len, const unsigned char* key);

	std::string EncryptS(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char* key);
	std::string EncryptS(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char* nonce, const unsigned char* key);
	std::string EncryptS(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char *salt,  uint64_t salt_len, const unsigned char* key);

	std::string EncryptBase64(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char* key);
	std::string EncryptBase64(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char* nonce, const unsigned char* key);
	std::string EncryptBase64(const unsigned char *plaintext, uint64_t plaintext_len,
			const unsigned char *salt,  uint64_t salt_len, const unsigned char* key);

	////////////////////  SEPERATOR ////////////////////////

	int Decrypt(const unsigned char *ciphertext, uint64_t ciphertext_len, unsigned char *plaintext,
			const unsigned char* key);
	int Decrypt(const unsigned char *ciphertext, uint64_t ciphertext_len, unsigned char *plaintext,
			const unsigned char* nonce, const unsigned char* key);
	int Decrypt(const unsigned char *ciphertext, uint64_t ciphertext_len, const unsigned char* salt,
			int salt_len, unsigned char* plaintext, const unsigned char* key);

	SharedSmartMem DecryptM(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* key);
	SharedSmartMem DecryptM(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* nonce, const unsigned char* key);
	SharedSmartMem DecryptM(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* salt, int salt_len, const unsigned char* key);

	std::string DecryptS(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* key);
	std::string DecryptS(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* nonce, const unsigned char* key);
	std::string DecryptS(const unsigned char *ciphertext, uint64_t ciphertext_len,
			const unsigned char* salt, int salt_len, const unsigned char* key);

	int DecryptBase64(const char* base64_str, unsigned char* plaintext, const unsigned char* key);
	int DecryptBase64(const char* base64_str, unsigned char* plaintext,
			const unsigned char* nonce, const unsigned char* key);
	int DecryptBase64(const char* base64_str, unsigned char* plaintext,
			const unsigned char* salt, int salt_len, const unsigned char* key);

	std::string DecryptBase64S(const char* base64_str, const unsigned char* key);
	std::string DecryptBase64S(const char* base64_str, const unsigned char* nonce,
			const unsigned char* key);
	std::string DecryptBase64S(const char* base64_str, const unsigned char* salt, int salt_len,
			const unsigned char* key);

	void HandleErrors(void);

private:
	unsigned char nonce_[crypto_aead_aes256gcm_NPUBBYTES];
	bool initiated_;
	bool strict_;
};

#endif /* MEKKA_CRYPTO_H_ */

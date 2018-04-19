/*
 * RSAPrivateKey.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */
#include <string>
#include "Base64.h"
#include "RSAPrivateKey.h"
#include "spdlog/fmt/fmt.h"

RSAPrivateKey::RSAPrivateKey(const char* key_path)
{
	bi_ = BIO_new(BIO_s_file());
	int ret = BIO_read_filename(bi_, key_path);
	pkey_ = nullptr;
	if (ret < 0 || (PEM_read_bio_PrivateKey(bi_, &pkey_, NULL, NULL) == NULL))
	{
		std::string errinfo = fmt::format("Failed to load PEM private key : {}", key_path);
		throw std::runtime_error(errinfo);
	}
}

RSAPrivateKey::~RSAPrivateKey()
{
	EVP_PKEY_free(pkey_);
	BIO_free(bi_);
}

std::string RSAPrivateKey::sign_signature(int type, unsigned char *m, unsigned int m_len)
{
	unsigned char buff[4096];
	unsigned int buff_len = sizeof(buff);

	RSA* rsa = pkey_->pkey.rsa;

	int ret = RSA_sign(NID_sha256, m, m_len, buff, &buff_len, rsa);
	if (ret != 1)
	{
		throw std::runtime_error("Failed to call RSA_sign.");
	}

	int b64_len = Base64encode_len(buff_len);
	assert(b64_len > 0);
	std::string b64_str(b64_len, '\0');
	char* encoded = const_cast<char*>(b64_str.data());
	Base64encode(encoded, reinterpret_cast<char*>(buff), buff_len);

	return b64_str;
}

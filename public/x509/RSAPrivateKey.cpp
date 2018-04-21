/*
 * RSAPrivateKey.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */
#include <string>
#include <openssl/err.h>
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

	cipher_ctx_ = EVP_CIPHER_CTX_new();
}

RSAPrivateKey::~RSAPrivateKey()
{
	EVP_CIPHER_CTX_free(cipher_ctx_);

	EVP_PKEY_free(pkey_);
	BIO_free(bi_);
}

std::string RSAPrivateKey::sign_signature(const char *m, unsigned int m_len)
{
	unsigned char buff[4096];
	unsigned int buff_len = sizeof(buff);

	RSA* rsa = pkey_->pkey.rsa;

	int ret = RSA_sign(NID_sha256, (unsigned char*)m, m_len, buff, &buff_len, rsa);
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

std::string RSAPrivateKey::unceal_text(const char* cealed_txt_str, int cealed_txt_len)
{
	int keyl = *(int*)cealed_txt_str;
	const char* cealed_aes_256_cbc_key = cealed_txt_str + sizeof(keyl);
	unsigned char* iv = (unsigned char*)(cealed_aes_256_cbc_key + keyl);

	unsigned char* ek = (unsigned char*)cealed_aes_256_cbc_key;
	int ekl = keyl;

	int ret = EVP_OpenInit(cipher_ctx_, EVP_aes_256_cbc(), ek, ekl, iv, pkey_);
	if (ret != 1)
	{
		throw std::runtime_error("Failed to call EVP_OpenInit.");
	}

	int actual_iv_len = EVP_CIPHER_iv_length(EVP_aes_256_cbc());
	int head_len = sizeof(keyl) + keyl + actual_iv_len;
	std::string uncealed_txt(cealed_txt_len - head_len, '\0');
	char* out = (char*)uncealed_txt.data();
	int outl = uncealed_txt.size();
	const unsigned char* in = (const unsigned char*)(cealed_txt_str + head_len);
	ret = EVP_OpenUpdate(cipher_ctx_, (unsigned char*)out, &outl, in, cealed_txt_len - head_len);
	if (ret != 1)
	{
		throw std::runtime_error("Failed to call EVP_OpenUpdate.");
	}

	int outl2 = uncealed_txt.size() - outl;
	ret = EVP_OpenFinal(cipher_ctx_, (unsigned char*)out + outl, &outl2);
	if (ret != 1)
	{
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("EVP_OpenFinal failed.");
	}
	uncealed_txt.resize(outl2 + outl);

	return uncealed_txt;
}

std::string RSAPrivateKey::unceal_text_base64(const char* cealed_txt_base64_str)
{
	int plain_txt_len = Base64decode_len(cealed_txt_base64_str);
	std::string plain_txt_str;
	plain_txt_str.resize(plain_txt_len);
	char* plain_txt = const_cast<char*>(plain_txt_str.data());
	int actual_plain_txt_len = Base64decode(plain_txt, cealed_txt_base64_str);
	plain_txt_str.resize(actual_plain_txt_len);

	return unceal_text(plain_txt, actual_plain_txt_len);
}

/*
 * X509.cpp
 *
 *  Created on: Apr 17, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include "x509/X509Crt.h"
#include "spdlog/fmt/fmt.h"
#include "Base64.h"

X509Crt::X509Crt()
{
}

X509Crt::X509Crt(const char* crt_path)
{
	init(crt_path);
}

X509Crt::X509Crt(const char* mem, int mem_len)
{
	init(mem, mem_len);
}

void X509Crt::init(const char* crt_path)
{
	crt_path_ = crt_path;
	bi_ = BIO_new(BIO_s_file());
	int ret = BIO_read_filename(bi_, crt_path);
	if (ret < 0)
	{
		std::string errinfo = fmt::format("Failed to load x509 certificate: {}.", crt_path_);
		throw std::runtime_error(errinfo);
	}
	internal_init();
}

void X509Crt::init(const char* mem, int mem_len)
{
	crt_as_str_.assign(mem, mem + mem_len);
	bi_ = BIO_new_mem_buf(const_cast<char*>(crt_as_str_.data()), mem_len);
	internal_init();
}

void X509Crt::internal_init()
{
	if ((x509_ = PEM_read_bio_X509_AUX(bi_, NULL, NULL, NULL)) == NULL)
	{
		std::string errinfo = fmt::format("Failed to read x509 certificate.");
		throw std::runtime_error(errinfo);
	}
	serial_ = get_serial_core();
	pkey_ = extract_pkey_core();
	rsa_key_ = pkey_->pkey.rsa;

	cipher_ctx_ = EVP_CIPHER_CTX_new();
	srand(time(NULL));
}

X509Crt::~X509Crt()
{
	EVP_CIPHER_CTX_free(cipher_ctx_);
	X509_free(x509_);
	EVP_PKEY_free(pkey_);
	BIO_free(bi_);
}

long X509Crt::get_serial()const
{
	return serial_;
}

long X509Crt::get_serial_core()const
{
	ASN1_INTEGER *serial = X509_get_serialNumber(x509_);

	BIGNUM *bn = ASN1_INTEGER_to_BN(serial, NULL);
	if (!bn)
	{
		BN_free(bn);
		throw std::runtime_error("unable to convert ASN1INTEGER to BN");
	}

	char *tmp = BN_bn2dec(bn);
	if (!tmp)
	{
		BN_free(bn);
		throw std::runtime_error("unable to convert BN to decimal string.");
	}

//	if (strlen(tmp) >= len)
//	{
//		fprintf(stderr, "buffer length shorter than serial number\n");
//		BN_free(bn);
//		OPENSSL_free(tmp);
//		return EXIT_FAILURE;
//	}

	long serial_num = strtol(tmp, NULL, 10);
	BN_free(bn);
	OPENSSL_free(tmp);

	return serial_num;
}

EVP_PKEY* X509Crt::extract_pkey_core()const
{
	int pubkey_algonid = OBJ_obj2nid(x509_->cert_info->key->algor->algorithm);

	if (pubkey_algonid != NID_rsaEncryption)
	{
		throw std::runtime_error("Only RSA is supported at present.");
	}

//	const char* sslbuf = OBJ_nid2ln(pubkey_algonid);
//	assert(strlen(sslbuf) < PUBKEY_ALGO_LEN);
//	strncpy(buf, sslbuf, PUBKEY_ALGO_LEN);

	EVP_PKEY *pkey = X509_get_pubkey(x509_);
	if (pkey == NULL)
	{
		throw std::runtime_error("unable to extract public key from certificate.");
//	    IFNULL_FAIL(pkey, "unable to extract public key from certificate");
	}

	RSA *rsa_key;
	char *rsa_e_dec, *rsa_n_hex;
	rsa_key = pkey->pkey.rsa;
	if (rsa_key == NULL)
	{
		throw std::runtime_error("unable to extract public key from certificate.");
//	    IFNULL_FAIL(rsa_key, "unable to extract RSA public key");
	}

	rsa_e_dec = BN_bn2dec(rsa_key->e);
	if (rsa_e_dec == NULL)
	{
		throw std::runtime_error("unable to extract rsa-exponent.");
//		IFNULL_FAIL(rsa_e_dec,  "unable to extract rsa exponent");
	}
	OPENSSL_free(rsa_e_dec);

	rsa_n_hex = BN_bn2hex(rsa_key->n);
	if (rsa_n_hex == NULL)
	{
		throw std::runtime_error("unable to extract rsa modulus.");
//   	IFNULL_FAIL(rsa_n_hex,  "unable to extract rsa modulus");
	}
	OPENSSL_free(rsa_n_hex);

//	std::shared_ptr<EVP_PKEY> shared_pkey(pkey, [=](EVP_PKEY* k){
//		EVP_PKEY_free(pkey);
//	});
//	return shared_pkey;

	return pkey;
}

std::string X509Crt::as_str()const
{
	assert(!(crt_path_.empty() && crt_as_str_.empty()));

	if (!crt_path_.empty())
	{
		std::ifstream fs(crt_path_);
		std::string str((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
		return str;
	}
	else
	{
		return crt_as_str_;
	}
}

int X509Crt::verify_signature(const char *m, unsigned int m_len, const char* signature_b64)const
{
	int len = Base64decode_len(signature_b64);
	std::vector<char> buff(len);
	int ret = Base64decode(buff.data(), signature_b64);
	buff.resize(ret);

	ret = RSA_verify(NID_sha256, (const unsigned char*)m, m_len,
			(const unsigned char*)buff.data(), buff.size(), rsa_key_);

	return ret;
}

std::string X509Crt::ceal_text(const char* m, int m_len)
{
	int pubkey_len = EVP_PKEY_size(pkey_);
	std::vector<char> ek_buffer(pubkey_len);
	char* ek[1] = { ek_buffer.data() };
	int ekl[1] = { pubkey_len };

	unsigned char iv[EVP_MAX_IV_LENGTH];
	int actual_iv_len = EVP_CIPHER_iv_length(EVP_aes_256_cbc());
	int r = rand();
	memcpy(iv, &r, sizeof(r));

	EVP_PKEY* pubk[1] = { pkey_ };
	int npubk = 1;

	int ret = EVP_SealInit(cipher_ctx_, EVP_aes_256_cbc(), (unsigned char**)ek, ekl, iv, pubk, npubk);
	if (ret != 1)
	{
		throw std::runtime_error("EVP_SealInit failed.");
	}

	std::vector<char> out(m_len * 2);
	int out_len = out.size();
	ret = EVP_SealUpdate(cipher_ctx_, (unsigned char*)out.data(), &out_len, (unsigned char*)m, m_len);
	if (ret != 1)
	{
		throw std::runtime_error("EVP_SealUpdate failed.");
	}

	int written_len2;
	unsigned char* out_data = (unsigned char*)out.data();
	ret = EVP_SealFinal(cipher_ctx_, out_data + out_len, &written_len2);
	if (ret != 1)
	{
		throw std::runtime_error("EVP_SealFinal failed.");
	}

	int total_written_len = written_len2 + out_len;

	int keyl = ekl[0];
	int cealed_txt_len = sizeof(keyl) + keyl + actual_iv_len + total_written_len;
	std::string cealed_txt_str;
	cealed_txt_str.resize(cealed_txt_len );
	char* cealed_txt_data = const_cast<char*>(cealed_txt_str.data());
	char* sentinel = cealed_txt_data;
	memcpy(sentinel, &keyl, sizeof(keyl));
	sentinel += sizeof(keyl);
	memcpy(sentinel, ek[0], keyl);
	sentinel += keyl;
	memcpy(sentinel, iv, actual_iv_len);
	sentinel += actual_iv_len;
	memcpy(sentinel, out.data(), total_written_len);

	return cealed_txt_str;
}

std::string X509Crt::ceal_text_base64(const char* m, int m_len)
{
	std::string cealed_txt_str = ceal_text(m, m_len);
	int cealed_txt_len = cealed_txt_str.size();
	int b64encode_len = Base64encode_len(cealed_txt_len);
	std::string cealed_txt_b64_str;
	cealed_txt_b64_str.resize(b64encode_len);
	const char* cealed_txt_data = cealed_txt_str.data();
	int ret = Base64encode(const_cast<char*>(cealed_txt_b64_str.data()), cealed_txt_data, cealed_txt_len);
	cealed_txt_b64_str.resize(ret);

	return cealed_txt_b64_str;
}

X509* X509Crt::get_x509_crt()const
{
	return x509_;
}

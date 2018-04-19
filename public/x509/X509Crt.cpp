/*
 * X509.cpp
 *
 *  Created on: Apr 17, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */
#include "x509/X509Crt.h"
#include <fstream>
#include <streambuf>
#include <stdexcept>
#include "spdlog/fmt/fmt.h"

X509Crt::X509Crt(const char* crt_path)
{
	crt_path_ = crt_path;
	bi_ = BIO_new(BIO_s_file());
	int ret = BIO_read_filename(bi_, crt_path);
	if (ret < 0 || ((x509_ = PEM_read_bio_X509_AUX(bi_, NULL, NULL, NULL)) == NULL))
	{
		std::string errinfo = fmt::format("Failed to load x509 certificate: {}", crt_path);
		throw std::runtime_error(errinfo);
	}
	serial_ = get_serial_core();
	pkey_ = extract_pkey_core();
	rsa_key_ = pkey_->pkey.rsa;
}

X509Crt::~X509Crt()
{
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
	std::ifstream fs(crt_path_);

	std::string str((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());

	return str;
}

int X509Crt::verify_signature(int type, unsigned char *m, unsigned int m_len,
			unsigned char *sigbuf, unsigned int siglen, RSA *rsa)const
{
	int ret = RSA_verify(NID_sha256, m, m_len, sigbuf, siglen, rsa_key_);

	return ret;
}

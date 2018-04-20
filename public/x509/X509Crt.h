/*
 * X509.h
 *
 *  Created on: Apr 17, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef PUBLIC_X509CRT_H_
#define PUBLIC_X509CRT_H_

#include <string>
#include <memory>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

class X509Crt
{
public:
	X509Crt(const char* crt_path);
	X509Crt(const char* mem, int mem_len);
	virtual ~X509Crt();

	long get_serial()const;

	std::string as_str()const;

	int verify_signature(const char *m, unsigned int m_len, const char* signature_b64)const;

	X509* get_x509_crt()const;

private:
	void internal_init();
	long get_serial_core()const;
	EVP_PKEY* extract_pkey_core()const;
//	std::shared_ptr<EVP_PKEY> extract_pkey_core()const;

private:
	BIO* bi_;
	X509* x509_;
	long serial_;
	EVP_PKEY* pkey_;
	RSA* rsa_key_;

	std::string crt_path_;
	std::string crt_as_str_;
};

#endif /* PUBLIC_X509CRT_H_ */

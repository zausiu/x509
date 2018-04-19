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
	virtual ~X509Crt();

	long get_serial()const;

	std::string as_str()const;

	int verify_signature(int type, unsigned char *m, unsigned int m_len,
			unsigned char *sigbuf, unsigned int siglen, RSA *rsa)const;

private:
	long get_serial_core()const;
	EVP_PKEY* extract_pkey_core()const;
//	std::shared_ptr<EVP_PKEY> extract_pkey_core()const;

private:
	BIO* bi_;
	X509* x509_;
	long serial_;
	std::string crt_path_;
	EVP_PKEY* pkey_;
	RSA* rsa_key_;
};

#endif /* PUBLIC_X509CRT_H_ */

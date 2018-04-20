/*
 * X509Verifier.h
 *
 *  Created on: Apr 18, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef PUBLIC_X509_X509VERIFIER_H_
#define PUBLIC_X509_X509VERIFIER_H_

#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
//#include "X509Crt.h"

class X509Verifier
{
public:
	X509Verifier(const char* ca_cert);
	virtual ~X509Verifier();

	bool is_legal(X509* cert);

private:
	BIO* bi_;
	X509_STORE* store_;
//	X509Crt ca_crt_;
};

#endif /* PUBLIC_X509_X509VERIFIER_H_ */

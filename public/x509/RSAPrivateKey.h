/*
 * RSAPrivateKey.h
 *
 *  Created on: Apr 18, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */

#ifndef PUBLIC_RSAPRIVATEKEY_H_
#define PUBLIC_RSAPRIVATEKEY_H_

#include <openssl/x509.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

class RSAPrivateKey
{
public:
	RSAPrivateKey(const char* key_path);
	virtual ~RSAPrivateKey();

	std::string sign_signature(const char *m, unsigned int m_len);

private:
	BIO* bi_;
	EVP_PKEY* pkey_;
};

#endif /* PUBLIC_RSAPRIVATEKEY_H_ */

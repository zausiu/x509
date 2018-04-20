/////////////////////////////////////////////////////////////
/// written by kamuszhou@tencent.com http://blog.ykyi.net
/// created on April 18, 2018
/////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <openssl/evp.h>
#include "x509/RSAPrivateKey.h"
#include "x509/X509Crt.h"
#include "x509/X509Verifier.h"

using namespace std;

int main(int argc, char** argv)
{
	OpenSSL_add_all_digests();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_algorithms();

	for (int i = 0; i < 100000; i++)
	{
		X509Crt ca_crt("/home/kamus/projs/openssl/ca.crt");
		cout << "serial: " << ca_crt.get_serial() << endl;

		X509Crt crt("/home/kamus/projs/openssl/test.crt");
		cout << "serial: " << crt.get_serial() << endl;

		std::string crt_as_str = crt.as_str();
		X509Crt crt_copy(crt_as_str.data(), crt_as_str.size());

		const char* msg = "hello, world.";
		int msg_len = strlen(msg);
		RSAPrivateKey pkey("/home/kamus/projs/openssl/test.key");
		std::string signature_b64 = pkey.sign_signature(msg, msg_len);

		int ret = crt_copy.verify_signature(msg, msg_len, signature_b64.c_str());
		cout << "signature verification: " << ret << "\n";

		X509Verifier verifier("/home/kamus/projs/openssl/ca.crt");
		cout << "ca certifcation verification: " << verifier.is_legal(ca_crt.get_x509_crt()) << "\n";
		cout << "test certifcation verification: " << verifier.is_legal(crt.get_x509_crt()) << "\n";

		cout << i << endl;
	}
	sleep(1);

	return 0;
}

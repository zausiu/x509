/////////////////////////////////////////////////////////////
/// written by kamuszhou@tencent.com http://blog.ykyi.net
/// created on April 18, 2018
/////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <openssl/evp.h>
#include "x509/RSAPrivateKey.h"
#include "x509/X509Crt.h"
#include "x509/X509Verifier.h"
#include "NeatUtilities.h"
#include "otter.h"

#define CRT_HOME "/home/kamus/projs/openssl/"
// #define CRT_HOME "/data/kamus/projs/openssl/"

using namespace std;

int main(int argc, char** argv)
{
//	for (int i = 0; i < 1; i++)
//	{
//		X509Crt ca_crt(CRT_HOME"ca.crt");
//		cout << "serial: " << ca_crt.get_serial() << endl;
//
//		X509Crt crt(CRT_HOME"test.crt");
//		cout << "serial: " << crt.get_serial() << endl;
//
//		std::string crt_as_str = crt.as_str();
//		X509Crt crt_copy(crt_as_str.data(), crt_as_str.size());
//
//		const char* msg = "hello, world.";
//		int msg_len = strlen(msg);
//		RSAPrivateKey pkey(CRT_HOME"test.key");
//		std::string signature_b64 = pkey.sign_signature(msg, msg_len);
//
//		int ret = crt_copy.verify_signature(msg, msg_len, signature_b64.c_str());
//		cout << "signature verification: " << ret << "\n";
//
//		X509Verifier verifier(CRT_HOME"ca.crt");
//		cout << "ca certifcation verification: " << verifier.is_legal(ca_crt.get_x509_crt()) << "\n";
//		cout << "test certifcation verification: " << verifier.is_legal(crt_copy.get_x509_crt()) << "\n";
//
//		std::string cealed_text = crt_copy.ceal_text_base64(msg, msg_len);
//		cout << "cealed_text: " << cealed_text << "\n";
//
//		std::string uncealed_text = pkey.unceal_text_base64(cealed_text.c_str());
//		cout << "uncealed_text: " << uncealed_text << "\n";
//
//		cout << i << endl;
//	}
//
//	cout << form_dayhour_uid(2018, 1, 2, 3) << "\n";

	char key[32];
	char salt[32];
	otter_init(CRT_HOME"test.crt", CRT_HOME"test.key");
	int ret = otter_get_key_and_salt(2018, 1, 1, 1, key, salt);
	assert(ret == 0);
	std::cout << "successfully got key and salt.\n";

	return 0;
}

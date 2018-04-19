/////////////////////////////////////////////////////////////
/// written by kamuszhou@tencent.com http://blog.ykyi.net
/// created on April 18, 2018
/////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "x509/RSAPrivateKey.h"
#include "x509/X509Crt.h"
#include "x509/X509Verifier.h"

using namespace std;

int main(int argc, char** argv)
{
	for (int i = 0; i < 1000000; i++)
	{
		X509Crt crt("/home/kamus/projs/openssl/test.crt");
		cout << "serial: " << crt.get_serial() << endl;

		RSAPrivateKey pkey("/home/kamus/projs/openssl/test.key");

		X509Verifier verifier("/home/kamus/projs/openssl/ca.crt");

		cout << i << endl;
	}
	sleep(10);

	return 0;
}

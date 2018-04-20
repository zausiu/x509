/*
 * X509Verifier.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: kamuszhou
 *  website: http://blog.ykyi.net
 */
#include <stdexcept>
#include "spdlog/fmt/fmt.h"
#include "X509Verifier.h"

X509Verifier::X509Verifier(const char* ca_crt_path)
//			:ca_crt_(ca_crt_path)
{
	store_ = X509_STORE_new();
	if (store_ == NULL)
	{
		throw std::runtime_error("unable to create new X509 store.");
	}
//	int rc = X509_STORE_add_cert(store_, ca_crt_.get_x509_crt());
	int rc = X509_STORE_load_locations(store_, ca_crt_path, nullptr);
	if (rc != 1) {
		std::string errinfo = fmt::format("unable to load certificates at {} to store.", ca_crt_path);
		throw std::runtime_error(errinfo);
	}
}

X509Verifier::~X509Verifier()
{
	X509_STORE_free(store_);
}

bool X509Verifier::is_legal(X509* cert)
{
	X509_STORE_CTX *ctx = X509_STORE_CTX_new();
	if (ctx == nullptr) {
		throw std::runtime_error("unable to create STORE CTX.");
	}
	if (X509_STORE_CTX_init(ctx, store_, cert, nullptr) != 1) {
		throw std::runtime_error("unable to initialize STORE CTX.");
		// X509_STORE_CTX_free(ctx);
	}
	int rc = X509_verify_cert(ctx);
	X509_STORE_CTX_free(ctx);

	return !!rc;
}


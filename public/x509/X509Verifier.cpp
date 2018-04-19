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

X509Verifier::X509Verifier(const char* ca_cert)
{
	store_ = X509_STORE_new();
	if (store_ == NULL)
	{
		throw std::runtime_error("unable to create new X509 store.");
	}

	int rc = X509_STORE_load_locations(store_, ca_cert, NULL);
	if (rc != 1) {
		std::string errinfo = fmt::format("unable to load certificates at {} to store.", ca_cert);
		throw std::runtime_error(errinfo);
	}
}

X509Verifier::~X509Verifier()
{
	X509_STORE_free(store_);
}


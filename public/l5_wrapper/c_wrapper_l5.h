//////////////////////////////////////////////////////////////
/// This l5 wrapper maintains a tcp connection pool.
/// Written by kamuszhou@tencent.com
/// Website http://blog.ykyi.net/
/// Created on March 13, 2018
//////////////////////////////////////////////////////////////

/** WARNING: NOT THREAD SAFE */

#pragma once

#include "cpp_wrapper_l5.h"

using namespace MyL5;

extern "C"
{
	L5Balancer* create_l5_balancer(int modid, int cmd);
	L5Route* create_l5_route(L5Balancer* l5b);

	void release_l5balancer(L5Balancer* l5b);
	void release_l5route(L5Route* route);

	int send_l5(L5Route* route, char* data, int len);
	int recv_l5(L5Route* route, void* buff, int len, int flags);

	const char* get_l5route_ip(L5Route* route);
	unsigned short get_l5route_port(L5Route* route);
}

//////////////////////////////////////////////////////////////
///// This l5 wrapper maintains a tcp connection pool.
///// Written by kamuszhou@tencent.com
///// Website http://blog.ykyi.net/
///// Created on March 13, 2018
////////////////////////////////////////////////////////////////

#include "c_wrapper_l5.h"
#include "cpp_wrapper_l5.h"

L5Balancer* create_l5_balancer(int modid, int cmd)
{
    L5Balancer* l5b = new L5Balancer(modid, cmd);     
    return l5b;
}

L5Route* create_l5_route(L5Balancer* l5b)
{
    L5Route* route = l5b->GetL5Route();
    return route;
}

void release_l5balancer(L5Balancer* l5b)
{
    delete l5b;
}

void release_l5route(L5Route* route)
{
    route->l5b.ReleaseL5Route(route);
}

int send_l5(L5Route* route, char* data, int len)
{
    return route->l5b.Send(route, data, len);
}

int recv_l5(L5Route* route, void* buff, int len, int flags)
{
    return route->l5b.Recv(route, buff, len, flags);
}

const char* get_l5route_ip(L5Route* route)
{
    return route->ip.c_str();
}

unsigned short get_l5route_port(L5Route* route)
{
    return route->port;
}


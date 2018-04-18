#include <iostream>
#include "cpp_wrapper_l5.h"
#include "c_wrapper_l5.h"

void test_cpp_wrapper()
{
    L5Balancer l5b(64200385, 65536);

    L5Route* route = l5b.GetL5Route();
    std::cout << "Got ip: " << route->ip << std::endl;
    std::cout << "Got port: " << route->port << std::endl;

    char buff[] = "abcd";
    int ret = l5b.Send(route, buff, 4);
    std::cout << "send returns: " << ret << std::endl;

    l5b.ReleaseL5Route(route);
}

void test_c_wrapper()
{
    L5Balancer* l5b = create_l5_balancer(64200385, 65536);
    L5Route* route = create_l5_route(l5b);

    std::cout << "got ip: " << get_l5route_ip(route) << "\n";
    std::cout << "got port: " << get_l5route_port(route) << "\n";
    
    char buff[] = "abcd";
    int ret = send_l5(route, buff, 4);
    std::cout << "send returns: " << ret << std::endl;
    
    release_l5balancer(l5b);
    release_l5route(route);
}

int main(int argc, char** argv)
{
    std::cout << "Run test for cpp_wrapper:\n";
    test_cpp_wrapper();

    std::cout << "\n\n##############\nRun test for c_wrapper:\n";
    test_c_wrapper();

    return 0;
}

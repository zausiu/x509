#!/usr/bin/env python3

import ctypes

l5 = ctypes.CDLL('./l5wrapper.so')
l5.create_l5_balancer.restype = ctypes.c_void_p
l5.create_l5_route.restype = ctypes.c_void_p
l5.get_l5route_ip.restype = ctypes.c_char_p

if __name__ == '__main__':
    print('invoke l5client_api using python3')
    l5b = l5.create_l5_balancer(64200385, 65536)
    for i in range(5):
        route = l5.create_l5_route(l5b)
        print('got ip:', l5.get_l5route_ip(route))
        print('got port:', l5.get_l5route_port(route))
        ret = l5.send_l5(route, b'abcd', 4)
        print('send_l5 returns:', ret)
        l5.release_l5route(route)
    l5.release_l5balancer(l5b)

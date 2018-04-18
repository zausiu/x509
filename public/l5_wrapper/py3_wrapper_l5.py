#!/usr/bin/env python3

import ctypes
import socket
import struct


def recv_all(sk, n):
    total = []
    remained = n
    while True:
        data = sk.recv(remained)
        total.append(data)
        if len(data) < remained:
            remained -= len(data)
        elif len(data) > remained:
            raise Exception('weired')
        else:
            break
    return b''.join(total)

l5 = ctypes.CDLL('./l5wrapper.so')
l5.create_l5_balancer.restype = ctypes.c_void_p
l5.create_l5_route.restype = ctypes.c_void_p
l5.get_l5route_ip.restype = ctypes.c_char_p


'''
The following are exported interface.
Refer to c_wrapper_l5.h for declaration of c function counterpart.
如下为导出的python3接口。
参考 c_wrapper_l5.h 获得对应C函数的声明。
'''
create_l5_balancer = l5.create_l5_balancer
create_l5_route = l5.create_l5_route
release_l5balancer = l5.release_l5balancer
release_l5route = l5.release_l5route
send_l5 = l5.send_l5
recv_l5 = l5.recv_l5
get_l5route_ip = l5.get_l5route_ip
get_l5route_port = l5.get_l5route_port


def roudtrip_bon(l5b, req_bon):
    '''
    A helper function that ask L5 for an ip port pair and create
    a tcp short connection, send one bon request and wait for a bon
    response.
    一个简单的帮助函数，从l5获得ip端口对后建立短连接，发一个bon请求
    再读一个bon回复。
    '''
    print('enter roudtrip_bon')
    route = create_l5_route(l5b)
    print('create_l5_route: ', route)
    port = get_l5route_port(route)
    print('get port:{}'.format(port))
    ip = get_l5route_ip(route).decode('ascii')
    print('get ip:{}'.format(ip))

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = (ip, port)
    sock.connect(server_address)
    sock.sendall(req_bon)
    stupid_bon_head = recv_all(sock, 5)
    stupid_bon_len = struct.unpack('<I', stupid_bon_head[1:])
    stupid_bon_body = recv_all(sock, stupid_bon_len - 5)

    return stupid_bon_head + stupid_bon_body


if __name__ == '__main__':
    print('invoke l5client_api using python3')
    l5b = l5.create_l5_balancer(64200385, 65536)
    for i in range(5):
        continue
        route = l5.create_l5_route(l5b)
        print('got ip:', l5.get_l5route_ip(route))
        print('got port:', l5.get_l5route_port(route))
        ret = l5.send_l5(route, b'abcd', 4)
        print('send_l5 returns:', ret)
        l5.release_l5route(route)

    roudtrip_bon(l5b, b'1234')

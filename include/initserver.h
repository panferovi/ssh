#pragma once

#include <netinet/in.h>

int init_tcp_server();
int init_udp_server();
int make_connection(int* sock, int connection, int port);

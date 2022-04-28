#pragma once

#include <netinet/in.h>

int parse(int argNum, char** args, int* client_acion, int* connection,
					  in_addr_t* ip, char** username, char** dst, char** src);

int connection_type(int argNum, char** args, int* connection);

int host_ip(int argNum, char** args, in_addr_t* ip);

int client_name(char* arg, char** username);

int dst_path(char* arg, char** dst);

int src_path(char* arg, char** src);

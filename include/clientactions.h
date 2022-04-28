#pragma once

#include <netinet/in.h>

int broadcast(int connection, int ip);

int sh(int connection, int ip, char* username);

int broadcast(int connection, int ip);

int copy(int connection, int ip, char* username, char* src, char* dst);

int msgsend(int sock, struct sockaddr_in *sockinfo, char* data, int nbytes);

int send_action(int sock, struct sockaddr_in *sockinfo, int action);

int send_username(int sock, struct sockaddr_in *sockinfo, char* username);

int send_fstat(int sock, struct sockaddr_in *sockinfo, mode_t fmode);

int client_authenticate(int sock, struct sockaddr_in* sockinfo);

int make_connection(int *sock, struct sockaddr_in* sockinfo, int connection, int ip);
#pragma once

#include <netinet/in.h>
#include <poll.h>
#include <pwd.h>

int broadcast(int sock, struct sockaddr_in *sockinfo);

int sh(int sock, struct sockaddr_in *sockinfo);

int copy(int sock, struct sockaddr_in *sockinfo);

int msgrecv(int sock, char* buf);

int recv_fmode(int sock, mode_t* fmode);

int recv_sh_cmd(int sock, int master, char* buf);

int send_sh_res(int sock, struct sockaddr_in* sockinfo, 
						  struct pollfd* shfd, char* buf);

int init_sh();

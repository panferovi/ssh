#include <netinet/in.h>
#include <poll.h>
#include <pwd.h>

void broadcast(int sock, struct sockaddr_in *sockinfo);

void sh(int sock, struct sockaddr_in *sockinfo);

void copy(int sock, struct sockaddr_in *sockinfo);

void recv_username(int sock, struct sockaddr_in *sockinfo, char* buf);

int recv_sh_cmd(int sock, int master, char* buf);

void send_sh_res(int sock, struct sockaddr_in* sockinfo, 
						   struct pollfd* shfd, char* buf);

int init_sh();

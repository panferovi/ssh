#include <netinet/in.h>

void broadcast(int sock, struct sockaddr_in *sockinfo);

void sh(int sock, struct sockaddr_in *sockinfo);

void copy(int sock, struct sockaddr_in *sockinfo);

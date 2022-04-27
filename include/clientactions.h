#include <netinet/in.h>

void sh(int connection, int ip, char* username);

void broadcast(int connection, int ip);

void copy(int connection, int ip, char* username, char* src, char* dst);

void send_action(int sock, struct sockaddr_in *sockinfo, int action);

void send_username(int sock, struct sockaddr_in *sockinfo, char* username);

void client_authenticate(int sock, struct sockaddr_in* sockinfo);

void msgsend(int sock, struct sockaddr_in *sockinfo, char* data, int nbytes);

struct sockaddr_in make_connection(int *sock, int connection, int ip);

// void set_quit(int _);
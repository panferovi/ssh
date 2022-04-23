#include <netinet/in.h>

const uint16_t PORT = 55555;

void init_tcp_server();
void init_udp_server();
void init_process(int listener, int connection);
int make_connection(int connection, int port);

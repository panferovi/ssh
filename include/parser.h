#include <netinet/in.h>

int parse(int argNum, char** args, int* connection, in_addr_t* ip, 
                                   char** username, char** dst, char** src);

int connection_type(int argNum, char** args);

in_addr_t host_ip(int argNum, char** args);

char* client_name(char* arg);

char* dst_path(char* arg);

char* src_path(char* arg);


int make_connection(int connection, struct sockaddr_in* sock_info);
void set_quit(int);
void sh(int connection, int ip, char* username);

void broadcast(int connection, int ip);

void copy(int connection, int ip, char* username, char* src, char* dst);

struct sockaddr_in make_connection(int *sock, int connection, int ip);

void set_quit(int _);
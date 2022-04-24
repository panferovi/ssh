#include <netdb.h>
#include <assert.h>
#include "actions.h"
#include "clientactions.h"

static const uint16_t PORT = 55555;

void sh(int connection, int ip, char* username)
{
	printf("making connection\n");
	int sock = 0;
	struct sockaddr_in sock_info = 
					make_connection(&sock, connection, ip);

	int action = SH;

	printf("sending message\n");
	sendto(sock, &action, sizeof(action), MSG_CONFIRM, (struct sockaddr*) &sock_info, sizeof(sock_info));
	printf("sent\n");
	close(sock);
}

void broadcast(int connection, int ip)
{
	int sock = 0;
	struct sockaddr_in sock_info = 
					make_connection(&sock, connection, ip);

	
}

void copy(int connection, int ip, char* username, char* src, char* dst)
{
	
}

struct sockaddr_in make_connection(int *sock, int connection, int ip)
{
	struct sockaddr_in sock_info = { .sin_family = AF_INET,
									 .sin_port   = htons(PORT),
									 .sin_addr   = {ip} };

	*sock = socket(AF_INET, connection, 0);

	if (connection == SOCK_STREAM && 
		connect(*sock, (struct sockaddr*) &sock_info, sizeof(sock_info)) == -1)
	{
		perror("connect");
		exit(-1);
	}

	return sock_info;
}

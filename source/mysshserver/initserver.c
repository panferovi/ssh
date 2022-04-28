#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "errorhandling.h"
#include "initserver.h"

static const uint16_t PORT = 8080;

extern int process_client(int sock);

int init_tcp_server()
{
	int tcpListener = 0;
	TRY (make_connection(&tcpListener, SOCK_STREAM, PORT));

	while (1)
	{
		int sock = accept(tcpListener, NULL, NULL);

		if (sock == -1)
		{
			log_perror();
			close(tcpListener);
			close(tcpListener);
			return -1;
		}

		switch (fork())
		{
			case -1:
				log_perror();
				close(tcpListener);
				close(tcpListener);
				return -1;
			case 0:
				close(tcpListener);
				TRY (process_client(sock));
				close(sock);
				return 0;
			default:
				close(sock);
				break;
		}
	}
	close(tcpListener);
	return 0;
}

int init_udp_server()
{
	int	udpListener = 0;
	TRY (make_connection(&udpListener, SOCK_DGRAM,  PORT));
	
	while(1) process_client(udpListener);
	
	close(udpListener);
	return 0;
}

int make_connection(int* sock, int connection, int port)
{
	*sock = socket(AF_INET, connection, 0);
	
	if (*sock == -1)
	{
		perror("socket");
		exit(-1);
	}

	struct sockaddr_in listener_info = { .sin_family = AF_INET, 
										 .sin_port   = htons(port), 
										 .sin_addr   = {htonl(INADDR_ANY)} };

	int opt = 1;

	TRY (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)));
	TRY (bind(*sock, (struct sockaddr*) &listener_info, sizeof(struct sockaddr_in)));

	if (connection == SOCK_STREAM)
		TRY (listen(*sock, 100));

	return 0;
}

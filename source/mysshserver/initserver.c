#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include "initserver.h"

static const uint16_t PORT = 8080;

extern void process_client(int sock);

void init_tcp_server()
{
	int tcpListener = make_connection(SOCK_STREAM, PORT);

	while (1)
	{
		int sock = accept(tcpListener, NULL, NULL);

		if (sock < 0)
		{
			perror("accept");
			exit(-1);
		}

		switch (0)
		{
			case -1:
				perror("fork");
				close(tcpListener);
				close(sock);
				exit(-1);
			case 0:
				close(tcpListener);
				process_client(sock);
				close(sock);
				exit(0);
			default:
				close(sock);
				break;
		}
	}
	close(tcpListener);
}

void init_udp_server()
{
	int	udpListener = make_connection(SOCK_DGRAM,  PORT);
	close(udpListener);
}

int make_connection(int connection, int port)
{
	int listener = socket(AF_INET, connection, 0);
	if (listener == -1)
	{
		perror("socket");
		exit(-1);
	}

	struct sockaddr_in listener_info = { .sin_family = AF_INET, 
										 .sin_port   = htons(port), 
										 .sin_addr   = {htonl(INADDR_ANY)} };

	int opt = 1;

	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
		perror("setsockopt");
		exit(-1);
	}

	if (bind(listener, (struct sockaddr*) &listener_info, sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind");
		exit(-1);
	}

	if (connection == SOCK_STREAM && (listen(listener, 100) == -1))
	{
		perror("listen");
		exit(-1);
	}

	return listener;
}

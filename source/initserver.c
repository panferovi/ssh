#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "initserver.h"

static const uint16_t PORT = 55555;

extern void process_user(int sock);

void init_tcp_server()
{
	int tcpListener = make_connection(SOCK_STREAM, PORT);
	init_process(tcpListener, SOCK_STREAM);
}

void init_udp_server()
{
	int	udpListener = make_connection(SOCK_DGRAM,  PORT);
	init_process(udpListener, SOCK_DGRAM);
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

void init_process(int listener, int connection)
{
	int sock = 0;

	while (1)
	{
		if (connection == SOCK_STREAM)
		{
			sock = accept(listener, NULL, NULL);

			if (sock < 0)
			{
				perror("accept");
				exit(-1);
			}

			switch (fork())
			{
				case -1:
					perror("fork");
					close(listener);
					exit(-1);
				case 0:
				{
					close(listener);
					process_user(sock);
					close(sock);
					exit(0);
				}
				default:
					break;
			}
		}
		else if (connection == SOCK_DGRAM)
			process_user(listener);
	}
}
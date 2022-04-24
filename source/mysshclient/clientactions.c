#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "actions.h"
#include "clientactions.h"



static const uint16_t PORT = 8080;

void broadcast(int connection, int ip)
{
	int sock = 0;
	struct sockaddr_in sockinfo = 
					make_connection(&sock, connection, ip);

    int opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) == -1)
	{
		perror("setsockopt");
		exit(-1);
	}
	
    send_action(sock, &sockinfo, BROADCAST);

	struct sockaddr_in server_info;
	socklen_t len = sizeof(server_info);
	char sock_ip[32] = "";
	
	time_t start = clock();
    
	while (clock() - start < CLOCKS_PER_SEC / 10)
	{
		if (recvfrom(sock, NULL, 0, MSG_DONTWAIT, (struct sockaddr*) &server_info, &len) != -1)
		{
			inet_ntop(AF_INET, &(server_info.sin_addr), sock_ip, INET_ADDRSTRLEN);
			printf("Server found IP %s\n", sock_ip);
		}
	}
}

void sh(int connection, int ip, char* username)
{
	int sock = 0;
	struct sockaddr_in sockinfo = 
					make_connection(&sock, connection, ip);

    send_action(sock, &sockinfo, SH);

	char buf[1024] = "";

	while (1)
	{
		printf("mysshclient: ");
		
		fgets(buf, 1024, STDIN_FILENO);

		if (sendto(sock, buf, strlen(buf), MSG_CONFIRM, 
							 (struct sockaddr*) &sockinfo, sizeof(sockinfo)) == -1)
		{
			perror("send");
			exit(-1);
		}
	
		if (recvfrom(sock, buf, strlen(buf), MSG_CONFIRM, NULL, NULL) == -1)
		{
			perror("recv");
			exit(-1);
		}
	}
}

void copy(int connection, int ip, char* username, char* src, char* dst)
{
	int sock = 0;
	struct sockaddr_in sockinfo = 
					make_connection(&sock, connection, ip);
	
    send_action(sock, &sockinfo, COPY);	
}

void send_action(int sock, struct sockaddr_in *sockinfo, int action)
{
    assert (sockinfo != NULL);

    if (sendto(sock, &action, sizeof(action), MSG_CONFIRM, 
                              (struct sockaddr*) sockinfo, sizeof(*sockinfo)) == -1)
    {
        perror("send");
        exit(-1);
    }
}

struct sockaddr_in make_connection(int *sock, int connection, int ip)
{
    assert (sock != NULL);

	struct sockaddr_in sockinfo = { .sin_family = AF_INET,
									 .sin_port   = htons(PORT),
									 .sin_addr   = {ip} };

	*sock = socket(AF_INET, connection, 0);

	if (connection == SOCK_STREAM && 
		connect(*sock, (struct sockaddr*) &sockinfo, sizeof(sockinfo)) == -1)
	{
		perror("connect");
		exit(-1);
	}

	return sockinfo;
}

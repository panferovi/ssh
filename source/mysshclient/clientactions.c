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

#define BUF_SIZE 4096

static const uint16_t PORT = 8080;

void msgsend(int sock, struct sockaddr_in *sockinfo, char* data, int nbytes);

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
	msgsend(sock, &sockinfo, username, strlen(username));
	client_authenticate(sock, &sockinfo);

	char buf[BUF_SIZE] = "";

	printf("\\h:\\w$ ");

	while (1)
	{
		memset(buf, '\0', BUF_SIZE);
		fgets(buf, BUF_SIZE, stdin);

		msgsend(sock, &sockinfo, buf, strlen(buf));

		if (!strcmp(buf, "exit\n"))
			break;

		memset(buf, '\0', BUF_SIZE);

		if (recvfrom(sock, buf, BUF_SIZE, MSG_CONFIRM, NULL, NULL) == -1)
		{
			perror("recv");
			exit(-1);
		}

		printf("%s", buf);
	}
}

void copy(int connection, int ip, char* username, char* src, char* dst)
{
	int sock = 0;
	struct sockaddr_in sockinfo = 
					make_connection(&sock, connection, ip);

    send_action(sock, &sockinfo, COPY);
	msgsend(sock, &sockinfo, username, strlen(username));
	client_authenticate(sock, &sockinfo);

	
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

void msgsend(int sock, struct sockaddr_in *sockinfo, char* data, int nbytes)
{
    assert (sockinfo != NULL);
    assert (data 	 != NULL);

	if (sendto(sock, data, nbytes, MSG_CONFIRM, 
					(struct sockaddr*) sockinfo, sizeof(*sockinfo)) == -1)
    {
        perror("send");
        exit(-1);
    }
}

void client_authenticate(int sock, struct sockaddr_in* sockinfo)
{
	assert(sockinfo != NULL);

	char buf[BUF_SIZE] = "";

	printf("Password: ");
	fgets(buf, BUF_SIZE, stdin);

	msgsend(sock, sockinfo, buf, strlen(buf));
	memset(buf, '\0', BUF_SIZE);

	if (recvfrom(sock, buf, BUF_SIZE, MSG_CONFIRM, NULL, NULL) == -1)
	{
		perror("recv");
		exit(-1);
	}

	printf("%s", buf);

	if (strcmp(buf, "login succesfull\n"))
		exit(-1);
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

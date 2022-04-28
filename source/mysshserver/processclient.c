#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include "actions.h"
#include "serveractions.h"
#include "errorhandling.h"

int recv_action(int sock, struct sockaddr_in *sockinfo);

int process_client(int sock)
{
	struct sockaddr_in sockinfo = {};

	int action = recv_action(sock, &sockinfo);

	switch (action)
	{
		case BROADCAST:
			broadcast(sock, &sockinfo);
			break;
		case SH:
			sh(sock, &sockinfo);
			break;
		case COPY:
			copy(sock, &sockinfo);
			break;
		default:
			log_error("Error: unexpected action\n");
			return -1;
	}
	return 0;
}

int recv_action(int sock, struct sockaddr_in *sockinfo)
{
	assert(sockinfo != NULL);

	int action = 0;
	socklen_t len = sizeof(struct sockaddr_in);

	TRY (recvfrom(sock, &action, sizeof(action), MSG_CONFIRM, 
								(struct sockaddr*) sockinfo, &len));

	return action;
}
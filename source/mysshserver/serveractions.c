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

void broadcast(int sock, struct sockaddr_in *sockinfo)
{
    int action = BROADCAST;

	if (sendto(sock, &action, sizeof(action), MSG_CONFIRM, 
              (struct sockaddr*) sockinfo, sizeof(struct sockaddr_in)) == -1)
	{
		perror("sendto");
		exit(-1);
	}
}

void sh(int sock, struct sockaddr_in* sockinfo)
{
    
}

void copy(int sock, struct sockaddr_in* sockinfo)
{
    
}
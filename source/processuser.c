#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "actions.h"


void process_user(int sock)
{
    char* message = getmsg(sock);
    
    parse(message);
    
    free(message);
}

char* getmsg(int sock)
{
	char* buf = calloc(1024, sizeof(char));

	if (recvfrom(sock, buf, 1024, MSG_CONFIRM, NULL, NULL) == -1)
	{
		perror("recvfrom");
		exit(-1);
	}

	

	return buf;
}

void parse (char* message)
{
    assert (message != NULL);

    
}
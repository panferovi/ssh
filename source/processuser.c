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
#include <fstream>

char* getmsg(int sock);

void process_user(int sock)
{
	printf("getting message\n");

    char* message = getmsg(sock);

	int action = *((int*)message);

	std::fstream log("log.txt", log.out | log.in | log.ate);

	log << "action: " << action << std::endl;

	printf("message = %d\n", action);

    // parse(message);
    
    free(message);
}

char* getmsg(int sock)
{
	char* buf = (char*) calloc(1024, sizeof(char));

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
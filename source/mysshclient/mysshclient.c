#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "actions.h"
#include "parser.h"
#include "logging.h"
#include "clientactions.h"
#include "errorhandling.h"

int main(int argc, char** argv)
{
	int	client_action, connection;
	in_addr_t ip;
	char *username = NULL, *dst = NULL, *src = NULL;

	TRY (parse(argc - 1, argv + 1, &client_action,
					&connection, &ip, &username, &dst, &src));

	switch (client_action)
	{
		case BROADCAST:
			TRY (broadcast(connection, ip));
			break;
		case SH:
			TRY (sh(connection, ip, username));
			break;
		case COPY:
			TRY (copy(connection, ip, username, src, dst));
			break;
		case ERR:
			log_error("Could not resolve hostname: Name or service not known\n");
			break;
	}

	free(username);
	free(dst);
	free(src);

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include "actions.h"
#include "parser.h"
#include "clientactions.h"

int main(int argc, char** argv)
{
	int	connection;
	in_addr_t ip;
	char *username = NULL, *dst = NULL, *src = NULL;

	int client_action = parse(argc - 1, argv + 1, 
								&connection, &ip, &username, &dst, &src);

	switch (client_action)
	{
		case BROADCAST:
			broadcast(connection, ip);
			break;
		case SH:
			sh(connection, ip, username);
			break;
		case COPY:
			copy(connection, ip, username, src, dst);
			break;
		case ERR:
			printf("Could not resolve hostname: Name or service not known\n");
			break;
	}

	free(username);
	free(dst);
	free(src);

	return 0;
}

// /void set_quit(int _) { sprintf(buf, "quit\n"); }
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

	int client_action = parse(argc - 1, argv + 1, &connection, &ip, &username, &dst, &src);

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

	// struct sigaction quit = {{set_quit}};
	// sigaction(SIGINT, &quit, NULL);

	// int sock = 0;

	// if (ip == htonl(INADDR_BROADCAST))
	// {
	// 	// make_broadcast();
	// }
	// else sock = make_connection(connection, &sockinfo);


	// // while (1)
	// // {
	// // 	fgets(buf, 512, stdin);


	// // if (sendto(sock, &buf, strlen(buf), MSG_CONFIRM, (struct sockaddr*)&sock_info_, sizeof(struct sockaddr_in)) == -1)
	// if (sendto(sock, &argc, sizeof(int), MSG_CONFIRM, NULL, 0) == -1)
	// {
	// 	perror("sendto");
	// 	exit(-1);
	// }

	// // 	if (!strcmp(buf, "exit\n"))
	// // 		break;
	// // }

	// close(sock);

	return 0;
}

// /void set_quit(int _) { sprintf(buf, "quit\n"); }
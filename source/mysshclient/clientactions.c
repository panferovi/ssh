#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <arpa/inet.h>
#include "actions.h"
#include "clientactions.h"
#include "errorhandling.h"
#include "logging.h"

#define BUF_SIZE 4096

static const uint16_t PORT = 8080;

int broadcast(int connection, int ip)
{
	int sock = 0;
	struct sockaddr_in sockinfo = {};

	TRYEXPR(make_connection(&sock, &sockinfo, connection, ip),
			"Couldn't make connection\n")

    int opt = 1;
	TRY (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)))
	
    TRY (send_action(sock, &sockinfo, BROADCAST))

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

	close(sock);
	return 0;
}

int sh(int connection, int ip, char* username)
{
	int sock = 0;
	struct sockaddr_in sockinfo = {};

	TRYEXPR(make_connection(&sock, &sockinfo, connection, ip),
			"Couldn't make connection\n");

    TRY (send_action(sock, &sockinfo, SH));
	TRY (msgsend(sock, &sockinfo, username, strlen(username)));

	TRY (client_authenticate(sock, &sockinfo));

	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_cc[VINTR] = 0;
	term.c_cc[VSUSP] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);

	char buf[BUF_SIZE] = "";

	printf("\\h:\\w$ ");

	while (1)
	{
		memset(buf, '\0', BUF_SIZE);
		fgets(buf, BUF_SIZE, stdin);

		TRY (msgsend(sock, &sockinfo, buf, strlen(buf)));

		if (!strcmp(buf, "exit\n"))
			break;

		memset(buf, '\0', BUF_SIZE);

		TRY (recvfrom(sock, buf, BUF_SIZE, MSG_CONFIRM, NULL, NULL));

		printf("%s", buf);
	}
	close(sock);
	return 0;
}

int copy(int connection, int ip, char* username, char* src, char* dst)
{
	int sock = 0;
	struct sockaddr_in sockinfo = {};
	
	TRYEXPR(make_connection(&sock, &sockinfo, connection, ip),
			"Couldn't make connection\n");

    TRY (send_action(sock, &sockinfo, COPY));

	TRY (msgsend(sock, &sockinfo, username, strlen(username)));
	TRY (client_authenticate(sock, &sockinfo));

	TRY (msgsend(sock, &sockinfo, dst, strlen(dst)));

	char buf[BUF_SIZE] = "";
	int srcfd = open(src, O_RDONLY);

	if (srcfd == -1)
	{
		perror("fstat");
		return -1;
	}

	struct stat srcinfo;

	TRY (fstat(srcfd, &srcinfo));

	TRY (send_fstat(sock, &sockinfo, srcinfo.st_mode));

	while(1)
	{
		memset(buf, '\0', BUF_SIZE);
		int rbytes = read(srcfd, buf, BUF_SIZE);

		if (rbytes == -1)
		{
			perror("read");
			return -1;
		}
		else if (rbytes == 0)
			break;

		TRY (msgsend(sock, &sockinfo, buf, rbytes));
	}
	close(sock);
	return 0;
}

int send_fstat(int sock, struct sockaddr_in *sockinfo, mode_t fmode)
{
    assert(sockinfo != NULL);

    TRY (sendto(sock, &fmode, sizeof(fmode), MSG_CONFIRM,
					(struct sockaddr*) sockinfo, sizeof(*sockinfo)));
	return 0;
}

int send_action(int sock, struct sockaddr_in *sockinfo, int action)
{
    assert(sockinfo != NULL);

    TRY (sendto(sock, &action, sizeof(action), MSG_CONFIRM, 
                              (struct sockaddr*) sockinfo, sizeof(*sockinfo)));
	return 0;
}

int msgsend(int sock, struct sockaddr_in *sockinfo, char* data, int nbytes)
{
    assert(sockinfo != NULL);
    assert(data 	!= NULL);

	TRY (sendto(sock, data, nbytes, MSG_CONFIRM, 
					(struct sockaddr*) sockinfo, sizeof(*sockinfo)));
	return 0;
}

int client_authenticate(int sock, struct sockaddr_in* sockinfo)
{
	assert(sockinfo != NULL);

	char buf[BUF_SIZE] = "";

	printf("Password: ");
	fgets(buf, BUF_SIZE, stdin);

	TRY (msgsend(sock, sockinfo, buf, strlen(buf)));
	memset(buf, '\0', BUF_SIZE);

	TRY (recvfrom(sock, buf, BUF_SIZE, MSG_CONFIRM, NULL, NULL))

	if (strcmp(buf, "login succesfull\n"))
	{
		printf("%s", buf);
		return -1;
	}

	return 0;
}

int make_connection(int *sock, struct sockaddr_in* sockinfo, int connection, int ip)
{
    assert (sock != NULL);

	sockinfo->sin_family 	  = AF_INET,
	sockinfo->sin_port   	  = htons(PORT),
	sockinfo->sin_addr.s_addr = ip;

	*sock = socket(AF_INET, connection, 0);

	if (connection == SOCK_STREAM)
		TRY (connect(*sock, (struct sockaddr*) &sockinfo, sizeof(sockinfo)))

	return 0;
}

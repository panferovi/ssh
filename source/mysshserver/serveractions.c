#define _GNU_SOURCE
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
#include <termios.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <sys/wait.h>
#include "serveractions.h"
#include "actions.h"
#include "errorhandling.h"

extern int login_client(int sock, struct sockaddr_in* sockinfo, 
								  struct passwd* client, char* username);

#define BUF_SIZE 4096

int broadcast(int sock, struct sockaddr_in *sockinfo)
{
    int action = BROADCAST;

	TRY (sendto(sock, &action, sizeof(action), MSG_CONFIRM, 
              (struct sockaddr*) sockinfo, sizeof(struct sockaddr_in)));

	return 0;
}

int sh(int sock, struct sockaddr_in* sockinfo)
{
	char buf[BUF_SIZE] = "";

	TRY (msgrecv(sock, buf));

	struct passwd client = {};
	TRY (login_client(sock, sockinfo, &client, buf));

	int master = 0;
	TRY (init_sh(&master));

	struct pollfd shfd = {.fd = master, .events = POLLIN};

	while (1)
	{
		if (recv_sh_cmd(sock, master, buf) == 0)
			break;

		TRY (send_sh_res(sock, sockinfo, &shfd, buf));
	}

	return 0;
}

int msgrecv(int sock, char* buf)
{
	assert(buf != NULL);

	memset(buf, '\0', BUF_SIZE);
	TRY (recvfrom(sock, buf, BUF_SIZE, MSG_CONFIRM, NULL, NULL));

	return 0;
}

int send_sh_res(int sock, struct sockaddr_in* sockinfo, 
						  struct pollfd* shfd, char* buf)
{
	assert(sockinfo != NULL);
	assert(shfd 	!= NULL);
	assert(buf 		!= NULL);

	memset(buf, '\0', BUF_SIZE);

	char output[BUF_SIZE] = "";
	int offset = 0;

	while (poll(shfd, 1, 100) != 0)
	{
		if (shfd->revents == POLLIN) 
		{
			int ret = read(shfd->fd, buf, BUF_SIZE);
			if (ret == -1)
			{
				log_perror();
				return -1;
			}
			memcpy(output + offset, buf, ret);
			offset += ret;
		}
	}

	TRY (sendto(sock, output, strlen(output), MSG_CONFIRM, 
				(struct sockaddr*) sockinfo, sizeof(struct sockaddr_in)));

	return 0;
}

int recv_sh_cmd(int sock, int master, char* buf)
{
	assert(buf != NULL);

	TRY (msgrecv(sock, buf));

	TRY (write(master, buf, BUF_SIZE))

	if (!strcmp(buf, "exit\n"))
	{
		wait(NULL);
		return 0;
	}

	return 0;
}

int init_sh(int* master)
{
	assert(master != NULL);

	char *bash_argv[] = {"sh", NULL};
	struct termios t;

	*master = posix_openpt(O_RDWR | O_NOCTTY);
	
	if (*master < 0) {
		log_error("openpt");
		return -1;
	}

	TRY (grantpt(*master));
	TRY (unlockpt(*master));
	TRY (tcgetattr(*master, &t));

	cfmakeraw(&t);

	TRY (tcsetattr(*master, TCSANOW, &t))

	if (fork() == 0)
	{
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		int term = open(ptsname(*master), O_RDWR);

		if (term < 0) 
		{
			log_error("open slave term");
			return -1;
		}

		dup2(term, STDIN_FILENO);
		dup2(term, STDOUT_FILENO);
		dup2(term, STDERR_FILENO);

		close(*master);

		execvp("sh", bash_argv);
	}

	return 0;
}

int copy(int sock, struct sockaddr_in* sockinfo)
{
	char buf[BUF_SIZE] = "";

	//get username
	msgrecv(sock, buf);
	struct passwd client;
	TRY (login_client(sock, sockinfo, &client, buf));

	//get path
	TRY (msgrecv(sock, buf));
	mode_t dstmode;
	TRY (recv_fmode(sock, &dstmode));

	int dstfd = open(buf, O_CREAT | O_TRUNC | O_WRONLY, dstmode);

	if (dstfd == -1)
	{
		log_perror();
		return -1;
	}

    int rbytes = 0;
	time_t start = clock();

	while (clock() - start < CLOCKS_PER_SEC / 10)
	{
		if ((rbytes = recvfrom(sock, buf, BUF_SIZE, MSG_DONTWAIT, NULL, NULL)) != -1)
		{
			start = clock();
			write(dstfd, buf, rbytes);
		}
	}

	close(dstfd);

	return 0;
}

int recv_fmode(int sock, mode_t* fmode)
{
	assert(fmode != NULL);

	TRY (recvfrom(sock, fmode, sizeof(*fmode), MSG_CONFIRM, NULL, NULL));

	return 0;
}
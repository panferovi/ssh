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

extern struct passwd login_client(int sock, struct sockaddr_in* sockinfo, char* username);

#define BUF_SIZE 4096

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
	char buf[BUF_SIZE] = "";

	// get username
	msgrecv(sock, buf);
	struct passwd client = login_client(sock, sockinfo, buf);
	
	int master = init_sh();

	struct pollfd shfd = {.fd = master, .events = POLLIN};

	while (1)
	{
		if (recv_sh_cmd(sock, master, buf) == 0)
			break;

		send_sh_res(sock, sockinfo, &shfd, buf);
	}
}

void msgrecv(int sock, char* buf)
{
	assert(buf != NULL);

	memset(buf, '\0', BUF_SIZE);

	if (recvfrom(sock, buf, BUF_SIZE, MSG_CONFIRM, NULL, NULL) == -1)
	{
		perror("recvfrom");
		exit(-1);
	}
}

void send_sh_res(int sock, struct sockaddr_in* sockinfo, 
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
				perror("write");
				exit(-1);
			}
			memcpy(output + offset, buf, ret);
			offset += ret;
		}
	}

	if (sendto(sock, output, strlen(output), MSG_CONFIRM, 
				(struct sockaddr*) sockinfo, sizeof(struct sockaddr_in)) == -1)
	{
		perror("recvfrom");
		exit(-1);
	}
}

int recv_sh_cmd(int sock, int master, char* buf)
{
	assert(buf != NULL);

	msgrecv(sock, buf);

	if (write(master, buf, BUF_SIZE) == -1)
	{
		perror("write");
		exit(-1);
	}

	if (!strcmp(buf, "exit\n"))
	{
		wait(NULL);
		return 0;
	}

	// return 1;
}

int init_sh()
{
	char *bash_argv[] = {"sh", NULL};
	struct termios t;
	int master;
	int ret;

	master = posix_openpt(O_RDWR | O_NOCTTY);
	if (master < 0) {
		perror("openpt");
		exit(-1);
	}

	if (grantpt(master)) {
		perror("grantpt");
		exit(-1);
	}

	if (unlockpt(master)) {
		perror("unlockpt");
		exit(-1);
	}

	if (tcgetattr(master, &t)) {
		perror("tcgetattr");
		exit(-1);
	}

	cfmakeraw(&t);

	ret = tcsetattr(master, TCSANOW, &t);
	if (ret) {
		perror("tcsetattr");
		exit(-1);
	}

	ret = fork();
	if (ret == 0) {
		int term;

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		term = open(ptsname(master), O_RDWR);
		if (term < 0) {
			perror("open slave term");
			exit(1);
		}

		dup2(term, STDIN_FILENO);
		dup2(term, STDOUT_FILENO);
		dup2(term, STDERR_FILENO);

		close(master);

		execvp("sh", bash_argv);
	}

	return master;
}

void copy(int sock, struct sockaddr_in* sockinfo)
{
	char buf[BUF_SIZE] = "";

	//get username
	msgrecv(sock, buf);
	struct passwd client = login_client(sock, sockinfo, buf);

	//get path
	msgrecv(sock, buf);
	mode_t dstmode;
	recv_fmode(sock, &dstmode);

	int dstfd = open(buf, O_CREAT | O_TRUNC | O_WRONLY, dstmode);

	if (dstfd == -1)
	{
		perror("open");
		exit(-1);
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
}

void recv_fmode(int sock, mode_t* fmode)
{
	assert(fmode != NULL);

	if (recvfrom(sock, fmode, sizeof(*fmode), MSG_CONFIRM, NULL, NULL) == -1)
	{
		perror("recvfrom");
		exit(-1);
	}
}
#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "initserver.h"
#include "errorhandling.h"

int main()
{
	pid_t pid = 0;//fork();

	if (pid == -1)
	{
		log_perror();
		return -1;
	}

	if (!pid)
	{
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		int ifd = open("/dev/null", O_RDONLY),
			ofd = open("/dev/null", O_RDWR),
			efd = open("/dev/null", O_RDWR);

		dup2(ifd, STDIN_FILENO);
		dup2(ofd, STDOUT_FILENO);
		dup2(efd, STDERR_FILENO);
		setsid();

		switch (0)
		{
			case -1:
				perror("fork");
				exit(-1);
			case 0:
		        TRY (init_tcp_server());
				break;
			default:
		        TRY (init_udp_server());
		}
	}
	return 0;
}

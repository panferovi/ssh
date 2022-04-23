#define _GNU_SOURCE

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main() {
	char *bash_argv[] = {"sh", NULL};
	struct termios t;
	char buf[2 << 20];
	int master;
	int ret;

	master = posix_openpt(O_RDWR | O_NOCTTY);
	if (master < 0) {
		perror("openpt");
		return 1;
	}

	if (grantpt(master)) {
		perror("grantpt");
		return 1;
	}

	if (unlockpt(master)) {
		perror("unlockpt");
		return 1;
	}

	ret = tcgetattr(master, &t);
	if (ret) {
		perror("tcgetattr");
		return 1;
	}

	cfmakeraw(&t);

	ret = tcsetattr(master, TCSANOW, &t);
	if (ret) {
		perror("tcsetattr");
		return 1;
	}

	ret = fork();
	if (ret == 0) {
		int term;

		setgid(0);

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

#define LS "ls -la /proc/self/fd\n"

	ret = write(master, LS, strlen(LS));
	if (ret != strlen(LS)) {
		perror("unable to write into master term");
		return 1;
	}

#define EXIT "exit\n"
	ret = write(master, EXIT, strlen(EXIT));
	if (ret != strlen(EXIT)) {
		perror("unable to write into master term");
		return 1;
	}

	wait(NULL);

	ret = read(master, buf, sizeof(buf));
	if (ret < 0) {
		perror("read");
		return 1;
	}

	ret = write(STDOUT_FILENO, buf, ret);
	if (ret < 0) {
		perror("write stdout");
		return 1;
	}

	close(master);

	return 0;
}



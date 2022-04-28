#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 1024
static char buf[BUF_SIZE];
int logfd = -1;

void init_log();

void print_log(char* str, ...)
{
    init_log();

    time_t now;
    time(&now);

	dprintf(logfd, "%s", ctime(&now));	

    va_list ap;
    va_start(ap, str);

    vdprintf(logfd, "%s", ap);

    va_end(ap);

    close(logfd);
}

void init_log()
{
    logfd = open("clientlog.txt", O_CREAT | O_RDWR | O_APPEND, 0666);

    if (logfd == -1)
    {
        perror("log open");
        exit(-1);
    }
}

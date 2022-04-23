#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int f_copy(char** path);

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("Too many arguments\n");
        return -1;
    }

    return f_copy(argv);
}

int f_copy(char** path)
{
    int f_src = open(path[1], O_RDONLY);

    if (f_src == -1)
    {
        perror("open");        
        close(f_src);
        return -1;
    }

    char buf[1024] = "";
    int r_bytes = 0;

    while(1)
    {
        r_bytes = read(f_src, &buf, 1024);

        if (r_bytes == 0)
            break;
        else if (r_bytes == -1)
        {
            perror("read");
            close(f_src);
            return -1;
        }

        if (send(buf) == -1)
        {
            perror("write");
            close(f_src);
            return -1;
        }
    }

    close(f_src);

    return 0;
}
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "parser.h"
#include "actions.h"

int parse(int argNum, char** args, int* connection, in_addr_t* ip, 
								   char** username, char** dst, char** src)
{
	assert(args != NULL);

	if (argNum < 1 || argNum > 3)
		return ERR;

	*connection = connection_type(argNum, args);

	if (*connection == -1)
	{
		printf("Wrong connection type: use \"TCP\" or \"UDP\"\n");
		exit(-1);
	}

	*ip = host_ip(argNum, args);

	if (*ip == 0)
	{
		printf("Wrong ip: usage example \"192.0.2.33\"\n" \
		 	    "You can use -broadcast to find server ip\n");
		exit(-1);
	}

	if (argNum == 1)
		return BROADCAST;

	*username = client_name(args[argNum - 1]);

	if (*username == NULL)
	{
		printf("Wrong username: usage example [-t <UDP|TCP>] [<user>@]<IP>");
		exit(-1);
	}	

	if (argNum == 3)
		return SH;

	*src = src_path(args[0]);
	*dst = dst_path(args[1]);

	if (*src == NULL || *dst == NULL)
	{
		printf("Wrong path: usage example /path/to/src user@10.35.57.4:/path/to/dst");
		exit(-1);
	}	

	return COPY;
}

int connection_type(int argNum, char** args)
{
	assert(args != NULL);

	if (argNum > 1 && !strcmp(args[0], "-t"))
	{
		if (!strcmp(args[1], "TCP"))
			return SOCK_STREAM;
		else if (!strcmp(args[1], "UDP"))
			return SOCK_DGRAM;
		else
			return -1;
	}
	return SOCK_DGRAM;
}

in_addr_t host_ip(int argNum, char** args)
{
	assert(args != NULL);

	if (argNum == 1 && !strcmp(args[0], "-broadcast"))
		return htonl(INADDR_BROADCAST);
	else if (argNum == 2 || argNum == 3)
	{
		char* ip_begin = strchr(args[argNum - 1], '@');

		if (ip_begin == NULL)
		{
			printf("Could not resolve hostname %s: " 
				   "Name or service not known\n", args[argNum - 1]);
			exit(-1);
		}
		
		ip_begin++;
		char* ip_str = ip_begin;

		if (argNum == 2)
		{
			char* ip_end = strchr(ip_str, ':');

			if (ip_end == NULL)
			{
				printf("Could not resolve comand: usage example " \
					   "/path/to/file [<user>@]<IP>:/path/to/file\n");
				exit(-1);
			}

			size_t ip_len = ip_end - ip_begin;
			ip_str = calloc(ip_len + 1, sizeof(char));
			memcpy(ip_str, ip_begin, ip_len);
		}

		struct in_addr ip;

		if (inet_pton(AF_INET, ip_str, &ip) == 0)
		{
			printf("Could not resolve hostname %s: " 
			"Name or service not known\n", ip_str);

			free(ip_str);

			exit(-1);
		}

		if (argNum == 2)
			free(ip_str);

		return ip.s_addr;
	}

	return 0;
}

char* client_name(char* arg)
{
	assert(arg != NULL);

	char* username_end  = strchr(arg, '@');
	size_t username_len = username_end - arg;

	char* username = calloc(username_len + 1, sizeof(char));

	memcpy(username, arg, username_len);
	return username;
}

char* dst_path(char* arg)
{
	assert(arg != NULL);

	char* dst_start  = strchr(arg, ':') + 1;
	char* dst = calloc(strlen(dst_start) + 1, sizeof(char));

	memcpy(dst, dst_start, strlen(dst_start));
	return dst;
}

char* src_path(char* arg)
{
	assert(arg != NULL);

	char* src = calloc(strlen(arg) + 1, sizeof(char));
	memcpy(src, arg, strlen(arg));

	return src;
}

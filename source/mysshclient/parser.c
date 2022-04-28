#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "parser.h"
#include "actions.h"
#include "errorhandling.h"

int parse(int argNum, char** args, int* client_acion, int* connection,
					  in_addr_t* ip, char** username, char** dst, char** src)
{
	assert(args != NULL);

	if (argNum < 1 || argNum > 3)
	{
		*client_acion = ERR;
		return 0;
	}

	TRYEXPR (connection_type(argNum, args, connection),
		"Wrong connection type: use \"TCP\" or \"UDP\"\n");

	TRYEXPR (host_ip(argNum, args, ip),
		"Wrong ip: usage example \"192.0.2.33\"\n" \
			"You can use -broadcast to find server ip\n");

	if (argNum == 1)
	{
		*client_acion = BROADCAST;
		return 0;
	}

	TRYEXPR (client_name(args[argNum - 1], username),
		"Wrong username: usage example [-t <UDP|TCP>] [<user>@]<IP>\n");

	if (argNum == 3)
	{
		*client_acion = SH;
		return 0;
	}

	TRY (src_path(args[0], src));
	TRY (dst_path(args[1], dst));
	*connection = SOCK_STREAM;

	if (*src == NULL || *dst == NULL)
	{
		log_error("Wrong path: usage example /path/to/src user@10.35.57.4:/path/to/dst\n");
		return -1;
	}	

	*client_acion = COPY;
	return 0;
}

int connection_type(int argNum, char** args, int* connection)
{
	assert(args != NULL);

	if (argNum > 1 && !strcmp(args[0], "-t"))
	{
		if (!strcmp(args[1], "TCP"))
			*connection = SOCK_STREAM;
		else if (!strcmp(args[1], "UDP"))
			*connection = SOCK_DGRAM;
		else
			return -1;
	}
	else *connection = SOCK_DGRAM;

	return 0;
}

int host_ip(int argNum, char** args, in_addr_t* ip)
{
	assert(args != NULL);

	if (argNum == 1 && !strcmp(args[0], "-broadcast"))
	{
		*ip = htonl(INADDR_BROADCAST);
		return 0;
	}
	else if (argNum == 2 || argNum == 3)
	{
		char* ip_begin = strchr(args[argNum - 1], '@');

		if (ip_begin == NULL)
		{
			log_error("Could not resolve hostname %s: " 
				   	  "Name or service not known\n", args[argNum - 1]);
			return -1;
		}
		
		ip_begin++;
		char* ip_str = ip_begin;

		if (argNum == 2)
		{
			char* ip_end = strchr(ip_str, ':');

			if (ip_end == NULL)
			{
				log_error("Could not resolve comand: usage example " \
					      "/path/to/file [<user>@]<IP>:/path/to/file\n");
				return -1;
			}

			size_t ip_len = ip_end - ip_begin;
			ip_str = calloc(ip_len + 1, sizeof(char));
			memcpy(ip_str, ip_begin, ip_len);
		}

		if (inet_pton(AF_INET, ip_str, ip) == 0)
		{
			log_error("Could not resolve hostname %s: " 
			 		   "Name or service not known\n", ip_str);

			free(ip_str);
			return -1;
		}

		if (argNum == 2)
			free(ip_str);

		return 0;
	}

	return -1;
}

int client_name(char* arg, char** username)
{
	assert(arg != NULL);

	char* username_end  = strchr(arg, '@');
	size_t username_len = username_end - arg;

	*username = calloc(username_len + 1, sizeof(char));
	
	if (*username == NULL)
	{
		log_perror();
		return -1;
	}
	
	memcpy(username, arg, username_len);

	return 0;
}

int dst_path(char* arg, char** dst)
{
	assert(arg != NULL);

	char* dst_start = strchr(arg, ':') + 1;
	*dst = calloc(strlen(dst_start) + 1, sizeof(char));

	if (*dst == NULL)
	{
		log_perror();
		return -1;
	}

	memcpy(dst, dst_start, strlen(dst_start));

	return 0;
}

int src_path(char* arg, char** src)
{
	assert(arg != NULL);

	*src = calloc(strlen(arg) + 1, sizeof(char));

	if (*src == NULL)
	{
		log_perror();
		return -1;
	}

	memcpy(src, arg, strlen(arg));

	return 0;
}

//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  ftpserverfunctions.cpp
//Description:  Supporting functions for FTP server


#include <stdio.h>
#include <errno.h>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using std::string;


string ResolveHostname(char *aHost)
{
	struct hostent *he;
	struct in_addr **address_list;
	
	if((he=gethostbyname(aHost)) == NULL)
	{
		herror("gethostbyname");
		exit(-1);
	}
	
	address_list = (struct in_addr **)he->h_addr_list;
	
	return inet_ntoa(*address_list[0]);
		
	
}

int GetSocket()
{
	int socketfd;
	if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return -1;
	}
	
	return socketfd;
	
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
	

//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  main.cpp
//Description:  Simple FTP client.
//The FTP client recognizes the following commands:
//List files at the client's current directory: 'lls'
//Display the client's current directory: 'lpwd'
//Change the client's current directory: 'lcd'
//Close connection: 'bye'

#include <iostream>
#include <istream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#define MAXDATASIZE 100

int main(int argc, char* argv[])
{
	int serverPort;
	struct sockaddr_in clientAddr;
	int sockfd;
	struct addrinfo hints, *serverinfo, *p;
	int ai;
	
	
	//check for the correct number of arguements
	if (argc != 3) {
	    fprintf(stderr,"usage: ftpclient hostname port_number\n");
	    exit(1);
	}
	
	
	memset(&hints, 0, sizeof hints);		//zero out struct
	hints.ai_family = AF_INET;			//IPV4
	hints.ai_socktype = SOCK_STREAM;		//TCP Socket type
	
	if ((ai = getaddrinfo(argv[1], argv[2], &hints, &serverinfo)) != 0) 
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ai));
			return 1;
		}
	
	// loop through all the results and connect to the first we can
	for(p = serverinfo; p != NULL; p = p->ai_next) 
		{
			if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
			{
				perror("client: socket");
				continue;
			}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}
	
	//~ inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
	//~ printf("client: connecting to %s\n", s);

	freeaddrinfo(serverinfo); // all done with this structure
	
	int len = 0;
	char buffer[MAXDATASIZE];
		
	while(1) 
	{  
		
		getline(buffer);
		len = strlen(buffer);
		//buffer[len] = '\0';
		printf("Sending %s (Total %d bytes)\n",buffer,len);
		
		//buffer[bytesToSend]='\0';
		if((len=send(sockfd, buffer, len, 0)) == -1)
		{
			perror("send");
			close(sockfd);
			exit(1);
		}
		
		
	}
	
	return EXIT_SUCCESS;
}
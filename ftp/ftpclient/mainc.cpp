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
#include "ftpclientfunctions.h"

#define MAXDATASIZE 100

using std::string;

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
			exit(1);
			continue;
		}

		break;
	}
	
	//~ inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
	//~ printf("client: connecting to %s\n", s);

	freeaddrinfo(serverinfo); // all done with this structure
	
	int len = 0;
	char buffer[MAXDATASIZE];
	string input;
		
	while(1) 
	{  
		
		gets(buffer);
		//len = strlen(buffer);
		
		//extract the actual command
		char *space, command[100], arguement[100];
		space = strtok(buffer, " ");
		strncpy(command,space, sizeof(command));
		len = strlen(command);
		command[len] ='\0';
		
		//extract the arguements
		if(space !=NULL)
		{
			space = strtok(NULL, " ");
			strncpy(arguement,space, sizeof(arguement));
			arguement[strlen(arguement)]='\0';
		}
		
		bool serverCommand = ((strncmp(command, "ls", 2) == 0) ||
							(strncmp(command, "pwd", 3) == 0) ||
							(strncmp(command, "cd", 2) == 0) ||
							(strncmp(command, "get", 3) == 0));
		
		bool clientCommand = ((strncmp(command, "help", 4) == 0) ||
							(strncmp(command, "lls", 3) == 0) ||
							(strncmp(command, "lpwd", 4) == 0) ||
							(strncmp(command, "lcd", 3) == 0));
		
		//process the entered command
		if (len == 0 || strncmp(command, "bye", 3) == 0)
		{
			printf("\nGoodbye\n\n");
			break;
		}//send server commands to the server
		else if(serverCommand)
		{
			printf("Server command: %s (%d)\n", command, len);
			if((len=send(sockfd, command, len, 0)) == -1)
			{
				perror("send");
				close(sockfd);
				exit(1);
			}
		}//process client commands locally
		else if(clientCommand)
		{
			if(strncmp(command, "help", 4) == 0)
			{
				PrintHelp();
			}
			else if(strncmp(command, "lls", 3) == 0)
			{
				system("ls");
			}
			else if(strncmp(command, "lpwd", 4) == 0)
			{
				system("pwd");
			}
			else if(strncmp(command, "lcd", 4) == 0)
			{
				system(strcat("cd ", arguement));
			}
			printf("Client command: %s (%d)\n", command, len);
		}//command not recognized
		else
		{
			std::cout << "Invalid command.  Type 'help' for a list of valid commands." << std::endl;
		}
		
				
	}
	close(sockfd);
	
	return EXIT_SUCCESS;
	
	
}
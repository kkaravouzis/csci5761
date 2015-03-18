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
	int bytesToReceive;
	char userInput[MAXDATASIZE];
	char buffer[MAXDATASIZE];
	char *space, command[MAXDATASIZE], arguement[MAXDATASIZE];
	string input;
	
	//zero out buffers
	memset(userInput, '\0', sizeof userInput);
	memset(buffer, '\0', sizeof buffer);
	memset(command, '\0', sizeof command);
	memset(arguement, '\0', sizeof arguement);
		
	while(1) 
	{  
		//get input from user
		std::cout << "ftpclient>";
		gets(userInput);
		userInput[strlen(userInput)] = '\0';
		strcpy(buffer, userInput);
		buffer[strlen(userInput)] = '\0';
		
		
		//extract the actual command
		space = strtok(userInput, " ");
		strncpy(command,space, sizeof(command));
		len = strlen(command);
		command[strlen(command)] ='\0';
		
		//extract the arguements
		if(space !=NULL)
		{
			space = strtok(NULL, " ");
			if(space !=NULL)
			{
				strncpy(arguement,space, sizeof(arguement));
				arguement[strlen(arguement)]='\0';
			}
		}
	
		
		//process the entered command
		if (len == 0 || strncmp(command, "bye", 3) == 0)
		{
			printf("\nGoodbye!\n\n");
			break;
		}//send SERVER COMMANDS to the server
		else if(isServerCommand(command))
		{
			if((len=send(sockfd, buffer, sizeof(buffer), 0)) == -1)
			{
				perror("send");
				close(sockfd);
				exit(1);
			}
			
			memset(buffer,'\0',sizeof(buffer));
			
			if(strncmp(command, "ls", 2) == 0) 	//LS COMMAND
			{
				bytesToReceive = recv(sockfd, buffer, 1000, 0);
				puts(buffer);
			}
			else if(strncmp(command, "pwd", 3) == 0)	//PWD COMMAND
			{				
				bytesToReceive = recv(sockfd, buffer, 1000, 0);
				puts(buffer);
			}
			else if(strncmp(command, "cd", 2) == 0)	//CD COMMAND
			{
				if(strlen(arguement) > 1)
				{
					bytesToReceive = recv(sockfd, buffer, 1000, 0);
					puts(buffer);
				}
			}
			else if(strncmp(command, "get", 3) == 0)	//GET COMMAND
			{
				
			}			
			
		}//process CLIENT COMMANDS locally
		else if(isClientCommand(command))
		{
			if(strncmp(command, "help", 4) == 0)		//HELP COMMAND
			{
				PrintHelp();
			}
			else if(strncmp(command, "lls", 3) == 0)	//LLS COMMAND
			{
				system("ls");
			}
			else if(strncmp(command, "lpwd", 4) == 0)	//LPWD COMMAND
			{
				puts(getwd(buffer));
			}
			else if(strncmp(command, "lcd", 3) == 0)	//LCD COMMAND
			{
				if((strlen(arguement) > 1) && strncmp(arguement, " ", 1) != 0)
				{
					if(chdir(arguement) == 0) system("pwd");
					else printf("%s is not a valid directory.\n", arguement);
				}
			}
		}//command not recognized
		else
		{
			std::cout << "Invalid command.  Type 'help' for a list of valid commands." << std::endl;
		}
		
				
	}
	close(sockfd);
	
	return EXIT_SUCCESS;
	
	
}
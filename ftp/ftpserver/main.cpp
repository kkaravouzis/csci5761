//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  main.cpp
//Description:  Simple FTP server that handles at least 5 concurrent connections.  
//The FTP server recognizes the following commands:
//List files at the server's current directory: 'ls'
//Display the server's current directory: 'pwd'
//Change the server's current directory: 'cd'
//Download files: 'get'

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include "ftpserverfunctions.h"


#define STARTPORT 5000  	// port # to start looking for open port
#define MAXPORT 65000 		//  maximum port number to search
#define MAXCONNECT 5		 // # of connections server will allow
#define MAXDATASIZE 10000
#define PATH_MAX 2048		//max length of directory path





int main(int argc, char* argv[])
{
	struct sockaddr_in serverAddr;
	struct addrinfo hints, *results, *p;
	int sockfd;
	struct sigaction sa;
	in_port_t serverPort = STARTPORT;
	
	
	//Create socket for incomming connections
	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0)
	{
		perror("socket failed");
	
	}
	
	//Create server address struct
	memset(&serverAddr, 0 , sizeof(serverAddr));  //zero out struct
	serverAddr.sin_family = AF_INET;                       //IPV4
	serverAddr.sin_port = htons(serverPort);            //Starting port #
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	
	//Find an open port and bind socket
	for(in_port_t p=serverPort; p < MAXPORT;)
	{
		if(bind(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr))<0)
		{
			p++;
			serverAddr.sin_port = htons(p);
			continue;
		}
		serverPort = p;
		
	}
	
	//Listen for incomming connections on socket
	if(listen(sockfd, MAXCONNECT) < 0)
	{
		perror("listen");
		exit(1);
	}
	
	//Reap all dead processes
	sa.sa_handler = sigchld_handler; 
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	
	printf("server: waiting for connections on port %d...\n", serverPort);

	
	//-----------------copied from example
	struct sockaddr_in their_addr; // connector's address information
	int new_fd, len;
	char s[INET6_ADDRSTRLEN];
	socklen_t sin_size;
	int bytesToReceive;
	char buffer[MAXDATASIZE];
	char sendBuffer[MAXDATASIZE];
	char arguement[MAXDATASIZE];
	char currentDir[PATH_MAX];
	char *result, *space, *ptrDir;
	
	ptrDir = getwd(currentDir);
	
	
	// main accept() loop
	while(1) {  
		//create a new socket and accept incomming connection on new socket
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.sin_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: connected from %s\n", s);
		
		//Create a child process for the new connection		
		if (!fork()) { 
			//close original socket from parent to allow additional connections
			//child will use new socket
			close(sockfd); 
			
			if (getsockname(new_fd, (struct sockaddr *)&their_addr, &sin_size) == -1)
				perror("getsockname");
			
			while(1)
			{
				//clear buffer and receive incomming data
				memset(buffer,'\0',sizeof(buffer));
				bytesToReceive = recv(new_fd, buffer, 128, 0);
				
				
				printf("Received:  %s\n", buffer);
				
				//extract the actual command
				space = strtok(buffer, " ");
				
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
				
				if(bytesToReceive < 0)
				{
					perror("recv");
					close(new_fd);
					exit(1);
				}else if(bytesToReceive == 0 || strncmp(buffer, "bye", 3) == 0)	//BYE or EMPTY STRING
				{
					printf("Client (%s) has been disconnected\n", (char *)inet_ntoa(their_addr.sin_addr));
					close(new_fd);
					exit(0);
				}else if(strncmp(buffer, "ls", 2) == 0)	//LS COMMAND
				{	
					if(strlen(arguement) > 1)
					{
						result = GetDirListing(arguement);
					}
					else
					{
						result = GetDirListing(currentDir);
					}
					
					printf("Results length: %d\n", strlen(result));
					
					if((len=send(new_fd, result, strlen(result), 0)) == -1)
					{
						perror("send");
						close(sockfd);
						exit(1);
					}
					
				}
				else if(strncmp(buffer, "pwd", 3) == 0)	//PWD COMMAND
				{
					ptrDir= getwd(currentDir);
					if((len=send(new_fd, ptrDir, strlen(ptrDir), 0)) == -1)
					{
						perror("send");
						close(sockfd);
						exit(1);
					}
				}
				else if(strncmp(buffer, "cd", 2) == 0)	//CD COMMAND
				{
					if(strlen(arguement) > 1)
					{
						if(chdir(arguement) == 0) 
						{
							result= getwd(currentDir);
						}
						else
						{
							result = strcat(arguement, " is not a valid directory.\n");
						}	
						
						if((len=send(new_fd, result, strlen(result), 0)) == -1)
						{
						perror("send");
						close(sockfd);
						exit(1);
						}
						
					}
					
					
					
				}
				else if(strncmp(buffer, "get", 3) == 0)	//GET FILE COMMAND
				{
					
				}
				
				//clear buffers
				memset(buffer,'\0',sizeof(buffer));
				memset(arguement,'\0',sizeof(arguement));
				
			}
			
			//Close socket when ftp session is completed
			close(new_fd); 
		}
		
	}
	
	//Ensure that socket is closed
	close(new_fd);
	return EXIT_SUCCESS;
}

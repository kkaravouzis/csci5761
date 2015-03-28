//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  main.cpp
//Description:  Simple FTP server that handles at least 5 concurrent connections.  
//The FTP server recognizes the following commands:
//List files at the server's current directory: 'ls'
//Display the server's current directory: 'pwd'
//Change the server's current directory: 'cd'
//Download files: 'get'

#include <cstdio>			//Standard input-output
#include <cstdlib>			//C standard library
#include <sys/socket.h>		//Socket structs library
#include <netinet/in.h>		//Internet domain address constants and structs
#include <netdb.h>		//Address info , getaddrinfo
#include <unistd.h>		//Misc Unix functions close(), chdir(),  fork(), etc.
#include <csignal>			//Signal handling
#include <cstring>			//C style strings
#include <string>			//C++ style strings
#include <iostream>		//cout, cin
#include "ftpserverfunctions.h"


#define MAXDATA 512			//maximum amount of data to send or receive
#define STARTPORT 5000			//port # to start looking for open port
#define MAXPORT 65000			//maximum port number to search
#define MAXCONNECT 5			//# of connections server will allow in wait queue
#define MAXPATH 1024			//maximum length of directory path

int main(int argc, char* argv[])
{
	struct sockaddr_in serverAddr;
	struct addrinfo hints;
	int sockfd;
	struct sigaction sa;
	in_port_t serverPort = STARTPORT;
	int yes = 1;
	
	//build server address structure
	bzero(&serverAddr, sizeof(serverAddr));			//zero out struct
	serverAddr.sin_family = AF_INET;				//IPV4
	serverAddr.sin_port	= htons(serverPort);			//starting port#
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	
	
	//create a reliable TCP stream socket for incomming connections
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("server: socket failed");
	}
	
	//allow server address to be reused
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) < 0) 
	{
		perror("server:  setsockopt");
		exit(1);
	}
	
	//find an open port and bind the socket
	for(in_port_t p = serverPort; p < MAXPORT;)
	{
		if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		{
			p++;
			serverAddr.sin_port = htons(p);
			continue;
		}
		serverPort = p;
	}
	
	//listen for incomming connections
	if(listen(sockfd, MAXCONNECT) < 0)
	{
		perror("server:  failure on listen");
		exit(1);
	}
	
	//reap dead processes
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("server:  sigaction failure");
		exit(1);
	}
	
	printf("server: waiting for connections on port %d...\n", serverPort);
	
	//buffers and variables to process ftp functions in main loop
	struct sockaddr_in clientAddr;
	int new_fd;
	int bytesToReceive = 0;
	long len = 0;
	char host[INET_ADDRSTRLEN];		//client host
	socklen_t sin_size;
	char buffer[MAXDATA];
	char command[MAXDATA];
	char arg[MAXDATA];
	char currentDir[MAXPATH];
	char *ptrDir, *space, *result;
	
	//store current working directory
	ptrDir = getcwd(currentDir,sizeof(currentDir));
	
	//main accept() loop
	while(true)
	{
		//create a new socket and accept incomming connections
		sin_size = sizeof(clientAddr);
		
		if((new_fd = accept(sockfd, (struct sockaddr *)&clientAddr, &sin_size)) < 0)
		{
			perror("server:  failed to accept connection");
			continue;
		}
		
		//get client information
		inet_ntop(clientAddr.sin_family,&clientAddr.sin_addr,host, sizeof(host));
		printf("server:  connection from host %s\n", host);
		
		//create a child process for the new connection
		if(!fork())
		{
			//close original socket from parent process to allow additional connections
			//child will use new socket
			close(sockfd);
			
			if(getsockname(new_fd, (struct sockaddr*)&clientAddr, &sin_size) < 0)
			{
				perror("server:  failed to get socket name");
			}
			
			//ftpserver functional loop that services the ftp client
			while(true)
			{
				//clear buffer and receive incomming data
				bzero(buffer, sizeof(buffer));
				bzero(arg, sizeof(arg));
				bytesToReceive = recv(new_fd, buffer, MAXDATA, 0);
				
				//extract the actual command
				space = strtok(buffer, " ");
				
				//extract the arguements
				if(space !=NULL)
				{
					space = strtok(NULL, " ");
					if(space !=NULL)
					{
						strncpy(arg,space, sizeof(arg));
						arg[strlen(arg)]='\0';
					}
				}
				
				if(bytesToReceive < 0)
				{
					perror("server:  receive error");
					close(new_fd);
					exit(1);
				}
				else if((bytesToReceive == 0) || (strncmp(command, "bye", 3) == 0) || (strncmp(command, "exit", 4) == 0)) 		//BYE OR EXIT COMMAND
				{
					printf("Client(%s) has disconnected\n", host);
					close(new_fd);
					exit(0);
				}
				else if(strncmp(buffer, "pwd", 3) == 0)							//PWD COMMAND
				{
					ptrDir= getcwd(currentDir,sizeof(currentDir));
					if((len=send(new_fd, ptrDir, strlen(ptrDir), 0)) == -1)
					{
						perror("send");
						close(sockfd);
						exit(1);
					}
				}
				else if(strncmp(buffer, "ls", 2) == 0)							//LS COMMAND
				{	
					if(strlen(arg) > 1)
					{
						if(PathExists(arg))
						{
							result = (char *)malloc(sizeof(GetDirListing(arg)));
							result = GetDirListing(arg);
						}
						else
						{
							result = (char *)malloc(64);
							result = strcat(arg, " is not a valid directory.");
						}
					}
					else
					{
						result = (char *)malloc(sizeof(GetDirListing(arg)));
						result = GetDirListing(currentDir);
					}
						
					if((len=send(new_fd, result, strlen(result), 0)) == -1)
					{
						perror("send");
						close(sockfd);
						exit(1);
					}
					
					
				}
				else if(strncmp(buffer, "cd", 2) == 0)							//CD COMMAND
				{
					if(strlen(arg) > 1)
					{
						if(chdir(arg) == 0) 
						{
							char str[] = "Current remote directory:  ";
							ptrDir = getcwd(currentDir,sizeof(currentDir));
							result = (char *)malloc(sizeof(currentDir)+64);
							result =strcat(str,currentDir);
						}
						else
						{
							result = (char *)calloc(128, sizeof(char));
							result = strcat(arg, " is not a valid directory.");
						}	
						
						if((len=send(new_fd, result, strlen(result), 0)) < 0)
						{
						perror("send");
						close(sockfd);
						exit(1);
						}
						
					}
				}
				else if(strncmp(buffer, "get", 3) == 0)							//GET FILE COMMAND
				{
					uint32_t size = 0;
					
					if(FileExists(arg))
					{
						//send file size to client
						size = FileSize(arg);
						uint32_t recvSize = 0;
						uint32_t fileSize = htonl((uint32_t)size);
						
						if((len =send(new_fd, &fileSize, sizeof(fileSize), 0)) < 0)
						{
							perror("server:  send");
							close(new_fd);
							exit(1);
						}
						
							printf("Server:  sending file (%s) %ld bytes to %s.\n", arg,size,host);
							//send file to client						
							FILE *file = fopen(arg, "r");
							char sendBuffer[size];
							bzero(sendBuffer, size);
							long sendBytes = 0;
							
							while((sendBytes = fread(sendBuffer, sizeof(char), size, file)) > 0)
							{
								if((len=send(new_fd, sendBuffer, size, 0)) <0)
								{
									perror("server:  send");
									close(sockfd);
									exit(1);
								}
							}
					}
					else
					{
						printf("%s has requested a file that does not exist.\n", host);
						if((len =send(new_fd, &size, sizeof(size), 0)) < 0)
						{
							perror("server:  send");
							close(new_fd);
							exit(1);
						}
					}
				}
				
			}
			close(new_fd);
		}
	}
	
	//Ensure that socket is closed
	close(new_fd);
	return EXIT_SUCCESS;
}
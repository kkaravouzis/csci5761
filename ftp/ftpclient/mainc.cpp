//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  main.cpp
//Description:  Simple FTP client.
//The FTP client recognizes the following commands:
//List files at the client's current directory: 'lls'
//Display the client's current directory: 'lpwd'
//Change the client's current directory: 'lcd'
//Close connection: 'bye'


#include <cstdio>			//Standard input-output
#include <cstdlib>			//C standard library
#include <sys/socket.h>		//Socket structs library
#include <netinet/in.h>		//Internet domain address constants and structs
#include <netdb.h>		//Address info , getaddrinfo
#include <unistd.h>		//Misc Unix functions close(), chdir(),  fork(), etc.
#include <cstring>			//C style strings
#include <string>			//C++ style strings
#include <iostream>		//cout, cin
#include "ftpclientfunctions.h"


#define MAXDATA 512


int main(int argc, char* argv[])
{
	in_port_t serverPort;
	char *serverName;
	struct sockaddr_in serverAddr;
	struct addrinfo hints, *serverinfo, *p;
	int aInfo;
	int sockfd;
	int ai;
	
	
	//check for the correct number of arguements
	if(argc !=3)
	{
		fprintf(stderr,"Usage:  ftpclient hostname port#\n.");
		exit(1);
	}
	
	
	//build the server address structure
	bzero(&serverAddr, sizeof(serverAddr));				//zero out structure
	serverPort = atoi(argv[2]);						//convert char string to integer
	serverAddr.sin_family = AF_INET;					//IPV4
	serverAddr.sin_port = htons(serverPort);				//add server's port #
	
	//resolve hostname of server
	bzero(&hints, sizeof(hints));						//zero out struct
	hints.ai_family = AF_INET;						//IPV4
	hints.ai_socktype = SOCK_STREAM;					//TCP, reliable stream socket
	
	serverName = argv[1];
	if((aInfo = getaddrinfo(serverName, argv[2], &hints, &serverinfo)) !=0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(aInfo));
		//perror("client: get address by name");
		exit(1);
	}
	
	
	
	// loop through all the results and connect to the first we can
	for(p = serverinfo; p != NULL; p = p->ai_next) 
	{
		//create a reliable TCP stream socket
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
		{
			perror("client: socket");
			continue;
		}

		//connect to server over TCP socket
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("client: connect");
			exit(1);
			continue;
		}

		break;
	}
	
	
	//release serverinfo struct
	freeaddrinfo(serverinfo);
	
	//buffers and variables to process ftp functions in main loop
	int bytesToReceive = 0;
	int len = 0;					//bytes to send
	char userInput[MAXDATA];		//char array to hold user input
	char command[MAXDATA];		//char array to hold command
	char arg[MAXDATA];				//char array to hold arguement
	char buffer[MAXDATA];
	char currentDir[MAXDATA];
	char *space, *ptrDir;
	
	ptrDir= getcwd(currentDir,sizeof(currentDir));
	
	//main loop for ftpclient program
	while(true)
	{
		//zero out buffers
		bzero(command, sizeof(command));
		bzero(currentDir, sizeof(currentDir));
		bzero(userInput, sizeof(userInput));
		bzero(arg, sizeof(arg));
		
		//get input from user.  do not accept blank input
		while(userInput[0] == '\0')
		{
			std::cout << "ftpclient>";
			fgets(userInput, MAXDATA-1, stdin);
			userInput[strlen(userInput)-1]='\0';
		}
		
		userInput[strlen(userInput)] = '\0';
		strcpy(buffer, userInput);
		buffer[strlen(buffer)] = '\0';
			
		//get command
		space = strtok(userInput, " ");
		strncpy(command, space, sizeof(command));
		command[strlen(command)] = '\0';
		
		//get arguement
		if(space != NULL)
		{
			space = strtok(NULL, " ");
			if(space != NULL)
			{
				strncpy(arg, space, sizeof(arg));
				arg[strlen(arg)] = '\0';
			}
		}
		
		
		//process commands entered by the user	
		if((strncmp(command, "bye", 3) == 0) || (strncmp(command, "exit", 4) == 0))		//BYE AND EXIT COMMANDS
		{
			printf("\nGoodbye!\n");
			break;
		}
		//send SERVER COMMANDS to the server
		else if(isServerCommand(command))
		{
			if(strncmp(command, "ls", 2) == 0)										//LS COMMAND
			{
				if((len=send(sockfd, buffer, sizeof(buffer), 0)) < 0)
				{
					perror("client: send error");
					close(sockfd);
					exit(1);
				}
				//clear buffer for receiving data
				bzero(buffer, MAXDATA);

				while(true)
				{
					bzero(buffer, MAXDATA);
					bytesToReceive = recv(sockfd, buffer, MAXDATA, 0);
					
					PrintLS(buffer);//printf("%s\n", buffer);
				
					if(bytesToReceive == 512)
					{
						bytesToReceive = recv(sockfd, buffer, MAXDATA, 0);
						PrintLS(buffer);//printf("%s\n", buffer);
					}
					else break;
				}
			}
			else if(strncmp(command, "pwd", 3) == 0)								//PWD COMMAND
			{
				if((len=send(sockfd, buffer, sizeof(buffer), 0)) < 0)
				{
					perror("client: send error");
					close(sockfd);
					exit(1);
				}
				//clear buffer for receiving data
				bzero(buffer, MAXDATA);

				bytesToReceive = recv(sockfd, buffer, MAXDATA, 0);
				printf("Current remote directory:  %s\n", buffer);
			}
			else if(strncmp(command, "cd", 2) == 0)									//CD COMMAND
			{
				if((len=send(sockfd, buffer, sizeof(buffer), 0)) < 0)
				{
					perror("client: send error");
					close(sockfd);
					exit(1);
				}
				//clear buffer for receiving data
				bzero(buffer, MAXDATA);

				if(strlen(arg) > 1)
				{
					bytesToReceive = recv(sockfd, buffer, MAXDATA, 0);
					puts(buffer);
				}
			}
			else if(strncmp(command, "get", 3) == 0)									//GET COMMAND
			{				
				//check if the file exist on the server and download if there is data to get
				if(!FileExists(arg))
				{
					if((len=send(sockfd, buffer, sizeof(buffer), 0)) < 0)
					{
						perror("client: send error");
						close(sockfd);
						exit(1);
					}
					//clear buffer for receiving data
					bzero(buffer, MAXDATA);

					//get filesize from server
					uint32_t size = 0;
					bytesToReceive = recv(sockfd, &size, sizeof(size), 0);
					
					//check to see if the file already exists on the client host
					if(size > 0)
					{
						uint32_t fileSize = ntohl((uint32_t)size);
						
						printf("Receiving %d bytes...\n", fileSize);
						int writeSize = 0;
						long totalReceived = 0;
						
						//save stream to file
						FILE *file = fopen(arg, "a");
						char rcvBuffer[fileSize];
						bzero(rcvBuffer, sizeof(rcvBuffer));
						
						while(totalReceived < fileSize)
						{
							bytesToReceive = recv(sockfd, rcvBuffer, sizeof(rcvBuffer), 0);
							writeSize = fwrite(rcvBuffer, sizeof(char), bytesToReceive, file);
							totalReceived += bytesToReceive;
						}
						
						
						fclose(file);
						printf("Success:  Downloaded %s\n", arg);
						
					}
					else 
					{	
						printf("File (%s) does not exit on the server.\n", arg);
					}
				}
				else
				{
					printf("Error:  %s already exists!\n", arg);
				}
			}
		}
		//process CLIENT COMMANDS locally
		else if(isClientCommand(command))
		{
			if(strncmp(command, "help", 4) == 0)										//HELP COMMAND
			{
				PrintHelp();
			}
			else if(strncmp(command, "lls", 3) == 0)									//LLS COMMAND
			{
				
				if(strlen(arg) > 1)
				{
					if(PathExists(arg))
						PrintLS(GetDirListing(arg));
					else
						printf("%s is not a valid directory.\n", arg); 
				}
				else
				{
					ptrDir= getcwd(currentDir,sizeof(currentDir));
					PrintLS(GetDirListing(currentDir));
				}
			}
			else if(strncmp(command, "lpwd", 4) == 0)								//LPWD COMMAND
			{
				ptrDir= getcwd(currentDir,sizeof(currentDir));
				printf("Current local directory:  %s\n", currentDir);
			}
			else if(strncmp(command, "lcd", 3) == 0)									//LCD COMMAND
			{
				if((strlen(arg) > 1) && strncmp(arg, " ", 1) != 0)
				{
					if(chdir(arg) == 0)
					{
						ptrDir= getcwd(currentDir,sizeof(currentDir));
						printf("Current local directory:  %s\n", currentDir);
					}
					else 
						printf("%s is not a valid directory.\n", arg);
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
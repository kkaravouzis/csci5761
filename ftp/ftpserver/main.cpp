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
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string>


#define STARTPORT 5000  	// port # to start looking for open port
#define MAXPORT 65000 		//  maximum port number to search
#define MAXCONNECT 5		 // # of connections server will allow
#define MAXDATASIZE 10000


void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char* argv[])
{
	struct sockaddr_in serverAddr;
	struct addrinfo hints, *results, *p;
	int rv;
	int sockfd;
	struct sigaction sa;
	in_port_t serverPort = STARTPORT;
	
	
	
	//addrinfo struct for getaddrinfo
	//~ memset(&hints, 0, sizeof hints);		//zero out struct
	//~ hints.ai_family = AF_INET;			//IPV4
	//~ hints.ai_socktype = SOCK_STREAM;		//TCP Socket type
	//~ hints.ai_flags = AI_PASSIVE; 			// use local IP
	
	//~ if ((rv = getaddrinfo(NULL, PORT, &hints, &results)) != 0) {
		//~ fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		//~ return 1;
	//~ }
	
	//Create socket for incomming connections
	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0)
	{
		perror("socket failed");
	
	}
	//std::cout << "Socket creation successful" << std::endl;
	
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
	
	//Listen for incomming connections of socket
	if(listen(sockfd, MAXCONNECT) < 0)
	{
		perror("listen");
		exit(1);
	}
	
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections on port %d...\n", serverPort);

	
	//-----------------copied from example
	struct sockaddr_in their_addr; // connector's address information
	int new_fd;
	char s[INET6_ADDRSTRLEN];
	socklen_t sin_size;
	int bytesToReceive;
	char buffer[MAXDATASIZE];
	
	
	// main accept() loop
	while(1) {  
		//sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.sin_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: connected from %s\n", s);
		
				
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			
			if (getsockname(new_fd, (struct sockaddr *)&their_addr, &sin_size) == -1)
				perror("getsockname");
			else
				printf("Serving %s on port number %d\n", s, ntohs(their_addr.sin_port));
			
			while(1)
			{
				memset(buffer,'\0',sizeof(buffer));
				bytesToReceive = recv(new_fd, buffer, 128, 0);
				if(bytesToReceive < 0)
				{
					perror("recv");
					close(new_fd);
					exit(1);
				}else if(bytesToReceive == 0 || strncmp(buffer, "bye", 3) == 0)
				{
					printf("Client (%s) has been disconnected\n", (char *)inet_ntoa(their_addr.sin_addr));
					close(new_fd);
					exit(0);
				}
				printf("Received:  %s\n", buffer);
			}
		}
		close(new_fd);  // parent doesn't need this
	}
	close(new_fd);
	return EXIT_SUCCESS;
}

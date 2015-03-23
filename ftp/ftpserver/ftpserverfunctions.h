//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  ftpserverfunctions.h
//Description:  Interface file defining the supporting functions for FTP server

#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using std::string;


string ResolveHostname(char *aHost);
//Precondition:  None
//Postcondition:  Prints a greeting to the standard output.


int GetSocket();


int BindSocket(int socketfd, sockaddr_in *serverAddress);

void *get_in_addr(struct sockaddr *sa);

void sigchld_handler(int s);

char *GetDirListing(char*);

long FileSize(char*);

bool FileExists(char*);
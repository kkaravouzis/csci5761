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
//Postcondition:  Returns an ip address, as a string, of the hostname passed as the arguement.


void sigchld_handler(pid_t s);
//Precondition:  None
//Postcondition:  Hanlder for SIGCHLD signal.  SIGCHLD indicates that a child process has terminated.


char *GetDirListing(char*);
//Precondition:  Takes a cstring for a linux/unix directory
//Postcondition:  Returns a tab separated cstring of files and directories.

long FileSize(char*);
//Precondition:  Takes a valid filename (must make sure that the file exists first)
//Postcondition:  Returns the size if the file in bytes.

bool PathExists(char *path);
//Precondition:  Takes a cstring for a linux/unix directory
//Postcondition:  Returns true if the path exists and user has access to it.  Otherwise false.

bool FileExists(char*);
//Precondition:  Takes cstring representing a filename
//Postcondition:  Returns true if the file exists and user has access to it.  Otherwise false.
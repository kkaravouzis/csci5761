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


void PrintHelp();
//Precondition:  None
//Postcondition:  Prints help to the screen.  List of commands and short description of what they do.

bool isServerCommand(char *c);
//Precondition:  Takes a user input command as a string.
//Postcondition:  Returns true if the command is for the server.

bool isClientCommand(char *c);
//Precondition:  Takes a user input command as a string.
//Postcondition:  Returns true if the command is for the client.

char *GetDirListing(char*);
//Precondition:  Takes a cstring for a linux/unix directory
//Postcondition:  Returns a tab separated cstring of files and directories.

bool PathExists(char *path);
//Precondition:  Takes a cstring for a linux/unix directory
//Postcondition:  Returns true if the path exists and user has access to it.  Otherwise false.

bool FileExists(char*);
//Precondition:  Takes cstring representing a filename
//Postcondition:  Returns true if the file exists and user has access to it.  Otherwise false.
void PrintLS(char *buf);
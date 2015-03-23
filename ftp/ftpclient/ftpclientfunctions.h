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

bool isServerCommand(char *c);

bool isClientCommand(char *c);

char *GetDirListing(char *path);

bool FileExists(char *f);
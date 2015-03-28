//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  ftpserverfunctions.cpp
//Description:  Supporting functions for FTP server


#include <stdio.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dirent.h>
#include <iostream>
#include <fstream>

using std::string;


string ResolveHostname(char *aHost)
{
	struct hostent *he;
	struct in_addr **address_list;
	
	if((he=gethostbyname(aHost)) == NULL)
	{
		herror("gethostbyname");
		exit(-1);
	}
	
	address_list = (struct in_addr **)he->h_addr_list;
	
	return inet_ntoa(*address_list[0]);
		
	
}


long FileSize(char *f)
{
	FILE *file;
	long size;

	file = fopen (f,"rb");
	
	fseek (file, 0, SEEK_END);
	size=ftell (file);
	fclose (file);
	
	return size;
	
}

bool FileExists(char *f)
{
	std::ifstream fileStream(f);
	return fileStream;
}

bool PathExists(char *path)
{
	DIR *dir = opendir(path);
	
	if(dir)
	{
		closedir(dir);
		return true;		
	}
	else
		return false;
	
}


char *GetDirListing(char *path)
{
	
	char *results;
	DIR *dir;
	struct dirent *DirEntry;
	dir = opendir(path);
	string listing = "";
	
	while(DirEntry=readdir(dir))
	{
		if (DirEntry->d_name[0] != '.')
		{
			listing = listing + DirEntry->d_name + "\t";
		}
				
	}
	closedir(dir);
	
	results = new char[listing.length()+1];
	strcpy(results, listing.c_str());
	return results;
}


void sigchld_handler(pid_t s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int GetSocket()
{
	int socketfd;
	if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return -1;
	}
	
	return socketfd;
	
}


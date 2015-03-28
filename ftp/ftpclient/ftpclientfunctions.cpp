//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  ftpserverfunctions.cpp
//Description:  Supporting functions for FTP server


#include <iostream>
#include <cstring>
#include <dirent.h>
#include <fstream>



using std::string;


void PrintHelp()
{
	std::cout << "\nftpclient commands\n"
				"bye\t- Quit ftplient program\n"
				"cd\t- Change directories on the server\n"
				"get\t- Download file from the remote server. get filename\n"
				"help\t- Show this helop list\n"
				"lcd\t- Change directories on the local computer\n"
				"lls\t- Show a list of files in the currentl local directory\n"
				"ls\t- Show a list of files in the current remote directory\n"
				"lpwd\t- Show the current directory on the local computer\n"
				"pwd\t- Show the current directory on the remote server\n"
			<< std::endl;
}

bool FileExists(char *f)
{
	std::ifstream fileStream(f);
	return fileStream;
}


bool isServerCommand(char *c)
{
	return ((strncmp(c, "ls", 2) == 0) ||
			(strncmp(c, "pwd", 3) == 0) ||
			(strncmp(c, "cd", 2) == 0) ||
			(strncmp(c, "get", 3) == 0));
	
}

bool isClientCommand(char *c)
{
	return ((strncmp(c, "help", 4) == 0) ||
			(strncmp(c, "lls", 3) == 0) ||
			(strncmp(c, "lpwd", 4) == 0) ||
			(strncmp(c, "lcd", 3) == 0));
	
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

void PrintLS(char *buf)
{
	//int width=0;
	//int count = 0;
	char *delim = strtok(buf, "\t");
	
	
	while(delim)
	{
		//count++;
		printf("\t%s\n", delim);
		delim = strtok(NULL, "\t");
		//~ if(delim != NULL)
		//~ {
			//~ if(strlen(delim) > width)
				//~ width = strlen(delim);
		//~ }
	}
}
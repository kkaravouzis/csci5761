#include <stdio.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dirent.h>
#include <iostream>

using std::string;

char *GetDirListing(char *path);

int main(int argc, char* argv[])
{
	std::cout << GetDirListing(argv[1]) << std::endl;
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
	std::cout << "Buffer Size:  " << strlen(results) << std::endl;
	return results;
}
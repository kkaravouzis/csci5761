//Author:  Kirk Karavouzis
//Course:  Computer Networks (CSCI 5761) / Spring 2015
//File:  ftpserverfunctions.cpp
//Description:  Supporting functions for FTP server


#include <iostream>


using std::string;


void PrintHelp()
{
	std::cout << "\nftpclient commands\n"
				"bye\n"
				"cd\n"
				"get\n"
				"help\n"
				"lcd\n"
				"lls\n"
				"ls\n"
				"lwd\n"
				"pwd\n"
			<< std::endl;
}
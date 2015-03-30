Author:  Kirk Karavouzis
Course:  Computer Networks (CSCI 5761) / Spring 2015
Description:  FTP is two separate C++ programs that consists of an ftp server and and ftp client.  Each will be compiled in its own folder, ftpserver and ftpclient respectively. 

ftpserver:  The ftpserver program will select an open TCP port to listen on beginning with port 5000 and incrementing by 1 until an open port is found and a TCP listening socket can be created.  The server responds to client commands pwd, ls, cd, get, bye and exit.  The makefile will compile and link the source files main.cpp and ftpserverfunctions.cpp.  The target executable is 'ftpserver'.  ftpserver takes no arguements.
    Example:  ./ftpserver

ftpclient:  The ftpclient program will make a TCP connection to the ftpserver in order to download text files.  The ftpclient program supports local commands lls, lcd, lpwd, and help.  It also sends commands pwd, ls, cd, get, bye, and exit to the server.
The makefile will compile and link the source files mainc.cpp and ftpclientfunctions.cpp.  The target executable is 'ftpclient'.  ftpclient takes two arguements.  The first arguement is the hostname of the server on which ftpserver is running.  The second arguement is the port number on which the ftpserver is listening.
    Example:  ./ftpclient example.com 5000
    
